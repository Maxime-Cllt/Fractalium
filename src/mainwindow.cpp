#include <QRubberBand>
#include "mainwindow.hpp"
#include <iostream>
#include <chrono>
#include <QApplication>
#include <MPI.hpp>
#include <QFileDialog>

using std::min;
using std::max;
using Fractalium::Double;

static unsigned short color_mode = 0;


struct color {
    int r, g, b;
};

auto get_color = [](const int &i) -> color {
    color c{};

    const double iteration = static_cast<double>(i) / MainWindow::TOTAL_COLORS;
    const double hue = 45.0 + 315.0 * iteration;
    const double C = 1.0 - std::abs(2.0 * iteration - 1.0);
    const double X = C * (1.0 - std::abs(std::fmod(hue / 60.0, 2.0) - 1.0));
    const double m = 0.0;

    switch (color_mode) {

        case 0:
            c.r = static_cast<int>((C + m) * 255);
            c.g = static_cast<int>((X + m) * 255);
            c.b = static_cast<int>(m * 255);
            break;

        case 1:
            c.r = static_cast<int>(m * 255);
            c.g = static_cast<int>((C * 0.5 + m) * 255);
            c.b = static_cast<int>(m * 255);
            break;

        case 2:
            c.r = static_cast<int>(m * 255);
            c.g = static_cast<int>((C + m) * 255);
            c.b = static_cast<int>((X + m) * 255);
            break;

        case 3:
            c.r = static_cast<int>(m * 255);
            c.g = static_cast<int>((X + m) * 255);
            c.b = static_cast<int>((C + m) * 255);
            break;

        case 4:
            c.r = static_cast<int>((X + m) * 255);
            c.g = static_cast<int>(m * 255);
            c.b = static_cast<int>((C + m) * 255);
            break;

        case 5:
            c.r = static_cast<int>((C + m) * 255);
            c.g = static_cast<int>(m * 255);
            c.b = static_cast<int>((X + m) * 255);
            break;

        case 6:
            c.r = static_cast<int>((C + m) * 255);
            c.g = static_cast<int>((C + m) * 255);
            c.b = static_cast<int>(m * 255);
            break;

        default:
            c.r = static_cast<int>((C + m) * 255);
            c.g = static_cast<int>((X + m) * 255);
            c.b = static_cast<int>(m * 255);
            break;
    }

    return c;
};

color c{};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    auto *central = new QWidget(this);
    this->setCentralWidget(central);


    _label = new Fractalium::FractalWidget(central);
    _label->setAlignment(Qt::AlignCenter);
    _label->setMinimumSize(DISPLAY_SIZE, DISPLAY_SIZE);
    _label->setMaximumSize(DISPLAY_SIZE, DISPLAY_SIZE);

    this->setMinimumSize(DISPLAY_SIZE, DISPLAY_SIZE);
    this->setMaximumSize(DISPLAY_SIZE, DISPLAY_SIZE);

    _color_map = std::vector<QColor>(MainWindow::TOTAL_COLORS);

    for (int i = 0; i < MainWindow::TOTAL_COLORS; i++) {
        c = get_color(i);
        _color_map[i] = QColor::fromRgb(c.r, c.g, c.b).name();
    }

    _color_map.emplace_back(QColor::fromRgb(0, 0, 0).name());
    _image = new QImage(_label->width(), _label->height(), QImage::Format_RGB32);
    _divergence_image = Fractalium::Image(_label->width(), _label->height());

    setupUi();

    _fractal = new Fractalium::Fractal();
    connect(_label, &Fractalium::FractalWidget::newSelection, this, &MainWindow::newSelection);
    Fractalium::MPICalculator::signal.connect([this] {
        qApp->postEvent(this, new PaintFractalEvent);
    });
}


/**
 * @brief Met à jour la couleur du fractal quand l'utilisateur change de couleur
 */
void MainWindow::updateColor() {
    for (int i = 0; i < MainWindow::TOTAL_COLORS; i++) {
        c = get_color(i);
        _color_map[i] = QColor::fromRgb(c.r, c.g, c.b).name();
    }
}

/**
 * @brief Peint le fractal dans l'image
 */
void MainWindow::paintFractal() {
    uint16_t start_x, end_x, end_y, start_y;
    start_x = 0;
    end_x = _divergence_image.width();
    start_y = 0;
    end_y = _divergence_image.height();

    for (uint16_t x = start_x; x < end_x; ++x) {
        for (uint16_t y = start_y; y < end_y; ++y) {
            if (_divergence_image.getPixel(x, y) == -1) continue;
            _image->setPixelColor(x, y, _color_map[min(_divergence_image.getPixel(x, y), MainWindow::TOTAL_COLORS)]);
        }
    }
    _back_history.emplace_back(history{*_image, _offset, _step_coord});
    _label->setFractal(*_image);
}

/**
 * @brief Met à jour le fractal en fonction de la nouvelle selection de l'utilisateur
 * @param start
 * @param end
 */
void MainWindow::newSelection(const QPoint &start, const QPoint &end) {

    // on calcule le nouveau offset x
    _offset.first =
            _offset.first + start.x() * _step_coord;
    // on calcule le nouveau offset y
    _offset.second =
            _offset.second + start.y() * _step_coord + (_step_coord * _label->height() / _label->width());

    // on calcule le nouveau step_coord
    int xDelta = abs(end.x() - start.x()), yDelta = abs(end.y() - start.y());
    _step_coord = Double(max(xDelta, yDelta)) / Double(_label->height() + _label->width()) *
                  _step_coord;


    Fractalium::MPICalculator::mpi_struct = Fractalium::MPIStruct(
            0, _label->width(),
            0, _label->height(),
            _offset.first,
            _offset.second,
            _label->width(), _label->height(),
            Fractalium::Fractal::ITERATIONS,
            _step_coord,
            *_fractal
    );

    Fractalium::MPICalculator::send(Fractalium::MPICalculator::mpi_struct, _divergence_image);
}

/**
 * @brief Initialise la barre de menu
 */
void MainWindow::setupUi() {

    _menu_bar = new QMenuBar(this);
    this->setMenuBar(_menu_bar);

    auto *menu = new QMenu("Fractal", _menu_bar);
    _menu_bar->addMenu(menu);

    auto *action = new QAction("Mandelbrot", menu);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        _offset = {-2.1, -2};
        _step_coord = 3.5 / _label->width();
        *_fractal = Fractalium::Fractal();
        mpiCalculate();
    });

    action = new QAction("Julia", menu);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        _offset = {-2.1, -2};
        _step_coord = 3.5 / _label->width();
        *_fractal = Fractalium::Fractal(Fractalium::Fractal::FractalType::Julia);
        mpiCalculate();
    });

    action = new QAction("BurningShip", menu);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        _offset = {-2.2, -2};
        _step_coord = 3.2 / _label->width();
        *_fractal = Fractalium::Fractal(Fractalium::Fractal::FractalType::BurningShip);
        mpiCalculate();
    });

    action = new QAction("Newton", menu);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        _offset = {-3, -3};
        _step_coord = 6.0 / _label->width();
        *_fractal = Fractalium::Fractal(Fractalium::Fractal::FractalType::Newton);
        mpiCalculate();
    });


    action = new QAction("Koch", menu);
    menu->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        _offset = {-5, -5};
        _step_coord = 10.0 / _label->width();
        *_fractal = Fractalium::Fractal(Fractalium::Fractal::FractalType::Koch);
        mpiCalculate();
    });

    auto *menu2 = new QMenu("History", _menu_bar);
    _menu_bar->addMenu(menu2);

    action = new QAction("Back", menu2);
    menu2->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::back);

    action = new QAction("Front", menu2);
    menu2->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::front);

    action = new QAction("Save", menu2);
    menu2->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::saveImage);

    auto menu3 = new QMenu("Couleur", _menu_bar);
    _menu_bar->addMenu(menu3);

    action = new QAction("Fire", menu3);
    menu3->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        color_mode = 0;
        updateColor();
    });

    action = new QAction("Green", menu3);
    menu3->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        color_mode = 1;
        updateColor();
    });

    action = new QAction("Light Green", menu3);
    menu3->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        color_mode = 2;
        updateColor();
    });

    action = new QAction("Blue", menu3);
    menu3->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        color_mode = 3;
        updateColor();
    });

    action = new QAction("Purple", menu3);
    menu3->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        color_mode = 4;
        updateColor();
    });

    action = new QAction("Pink", menu3);
    menu3->addAction(action);
    connect(action, &QAction::triggered, this, [this]() {
        color_mode = 5;
        updateColor();
    });

}

/**
 * @brief Gère les évènements de la fenêtre
 * @param event
 * @return
 */
bool MainWindow::event(QEvent *event) {
    if (event->type() == PaintFractalEvent::PaintFractalEventType) {
        paintFractal();
        return true;
    }
    if (event->type() == QEvent::Close) {
        Fractalium::MPICalculator::stop();
    }
    return QMainWindow::event(event);
}

/**
 * @brief Calcule le fractal en utilisant MPI
 */
void MainWindow::mpiCalculate() {
    Fractalium::MPICalculator::mpi_struct = Fractalium::MPIStruct(
            0, _label->width(),
            0, _label->height(),
            _offset.first,
            _offset.second,
            _label->width(), _label->height(),
            Fractalium::Fractal::ITERATIONS,
            _step_coord,
            *_fractal
    );
    Fractalium::MPICalculator::send(Fractalium::MPICalculator::mpi_struct, _divergence_image);
}

void MainWindow::back() {
    if (_back_history.size() <= 1)
        return;
    _front_history.emplace_back(history{*_image, _offset, _step_coord});
    _back_history.pop_back();
    auto h = _back_history.back();
    _offset = h.offset;
    _step_coord = h.step_coord;
    *_image = h.image;
    _label->setFractal(*_image);
}

/**
 * @brief Retourne en avant dans l'historique
 */
void MainWindow::front() {
    if (_front_history.empty())
        return;
    auto h = _front_history.back();
    _front_history.pop_back();
    _offset = h.offset;
    _step_coord = h.step_coord;
    *_image = h.image;
    _back_history.emplace_back(history{*_image, _offset, _step_coord});
    _label->setFractal(*_image);
}

/**
 * @brief Sauvegarde l'image
 */
void MainWindow::saveImage() {
    auto fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                                 QDir::homePath() + "/fractal.png",
                                                 tr("Images (*.png *.xpm *.jpg)"));
    if (fileName.isEmpty()) return;
    _image->save(fileName);
}