//
// Created by sr-71 on 30/12/2023.
//

#include "SettingsDialog.hpp"

#include <QLayout>
#include <QFormLayout>
#include <QLabel>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QSlider>
#include <QFileDialog>
#include <QComboBox>

using Fractalium::Settings;

/**
 * @brief Constructeur de la classe SettingsDialog
 * @param parent Widget parent
 */
SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {

    _size = QSize(Settings::DISPLAY_SIZE_WIDTH, Settings::DISPLAY_SIZE_HEIGHT);

    _iterations = static_cast<int>(Settings::ITERATIONS);
    _auto_snapshots = Settings::AUTO_SNAPSHOTS;
    _save_path = QString::fromStdString(Settings::SAVE_PATH);


    setWindowTitle("Paramètres");

    _layout = new QVBoxLayout(this);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    _layout->setSpacing(30);

    initUi();

}

/**
 * @brief Fonction d'initialisation de l'interface graphique

 */
void SettingsDialog::initUi() {
    initResizeUi();
    initIterationsUi();
    initAutoSnapshotsUi();
    initAreaAlgorithmTypeUi();

    auto github = new QLabel(this);
    github->setText("<a href=\"" + QString::fromStdString(Settings::GITHUB_URL) + "\">Github</a>");
    github->setTextFormat(Qt::RichText);
    github->setOpenExternalLinks(true);
    github->setTextInteractionFlags(Qt::TextBrowserInteraction);
    _layout->addWidget(github, Qt::AlignCenter);

    auto save = new QPushButton("Enregistrer", this);
    save->setDefault(true);
    _layout->addWidget(save, Qt::AlignBottom | Qt::AlignHCenter);

    connect(save, &QPushButton::clicked, this, [this] {
        Settings::DISPLAY_SIZE_WIDTH = _size.width();
        Settings::DISPLAY_SIZE_HEIGHT = _size.height();
        Settings::ITERATIONS = _iterations;
        Settings::AUTO_SNAPSHOTS = _auto_snapshots;
        Settings::SAVE_PATH = _save_path.toStdString();
        Settings::AREA_ALGORITHM_TYPE = area_algorithm_type;
        Settings::saveSettings();
        close();
    });

}

/**
 * @brief Fonction d'initialisation de l'interface graphique pour le redimensionnement de la fenêtre
 */
void SettingsDialog::initResizeUi() {
// Resize
    auto resize = new QGroupBox("Redimensionner la fenêtre principale", this);

    auto resize_layout = new QFormLayout(resize);
    resize_layout->setFieldGrowthPolicy(QFormLayout::FieldGrowthPolicy::AllNonFixedFieldsGrow);

    auto width = new QSpinBox(this);
    width->setRange(400, 5120);
    width->setValue(Settings::DISPLAY_SIZE_WIDTH);
    resize_layout->addRow("width", width);

    connect(width, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int val) {
        _size.setWidth(val);
    });


    auto height = new QSpinBox(this);
    height->setRange(400, 5120);
    height->setValue(Settings::DISPLAY_SIZE_HEIGHT);
    resize_layout->addRow("height", height);

    connect(height, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int val) {
        _size.setHeight(val);
    });

    _layout->addWidget(resize);
}

/**
 * @brief Fonction d'initialisation de l'interface graphique pour le nombre d'itérations

 */
void SettingsDialog::initIterationsUi() {
    // Iterations
    auto iterations = new QGroupBox("Nombre d'itérations", this);

    auto layout = new QGridLayout(iterations);

    auto label_min = new QLabel("min : 10 ", this);
    int max = std::numeric_limits<int>::max();
    auto label_max = new QLabel("max : " + QString::number(max) + " ", this);
    auto label_current = new QLabel(this);
    label_current->setAlignment(Qt::AlignCenter);

    auto iteration = new QSlider(Qt::Orientation::Horizontal, this);
    iteration->setRange(10, max);
    auto iteration_spin = new QSpinBox(this);
    iteration_spin->setRange(10, max);


    connect(iteration, &QSlider::valueChanged, this, [=, this](int val) {
        label_current->setText("value : " + QString::number(val));
        iteration_spin->setValue(val);
        _iterations = val;
    });

    connect(iteration_spin, QOverload<int>::of(&QSpinBox::valueChanged), this, [=, this](int val) {
        label_current->setText("value : " + QString::number(val));
        iteration->setValue(val);
        _iterations = val;
    });

    iteration->setValue(static_cast<int>(Settings::ITERATIONS));


    layout->addWidget(label_min, 0, 0, 1, 1);
    layout->addWidget(iteration, 0, 1, 1, 3);
    layout->addWidget(label_max, 0, 4, 1, 1);
    layout->addWidget(label_current, 1, 0, 1, 5);
    layout->addWidget(iteration_spin, 1, 0, 1, 5);

    _layout->addWidget(iterations);
}

/**
 * @brief Fonction d'initialisation de l'interface graphique pour les instantanés automatiques

 */
void SettingsDialog::initAutoSnapshotsUi() {
    // AutoSnapshots
    auto auto_snapshots = new QGroupBox("Instantanés automatiques", this);

    auto auto_snapshots_layout = new QVBoxLayout(auto_snapshots);

    auto_snapshots_layout->addWidget(new QLabel(
            "Cette fonctionnalité permet de générer des instantanés automatiques à chaque création de fractale ou de zoom.",
            this));

    auto auto_snapshots_check = new QCheckBox("Activer", this);
    auto_snapshots_check->setChecked(Settings::AUTO_SNAPSHOTS);
    auto_snapshots_layout->addWidget(auto_snapshots_check);

    connect(auto_snapshots_check, &QCheckBox::stateChanged, this, [this](int state) {
        _auto_snapshots = state == Qt::Checked;
    });

    auto_snapshots_layout->addWidget(new QLabel("Chemin de sauvegarde", this));

    auto auto_snapshots_path_layout = new QHBoxLayout;
    auto_snapshots_layout->addLayout(auto_snapshots_path_layout);

    auto auto_snapshots_path = new QLineEdit(this);
    auto_snapshots_path->setText(QString::fromStdString(Settings::SAVE_PATH));
    auto_snapshots_path_layout->addWidget(auto_snapshots_path);

    auto auto_snapshots_path_button = new QPushButton("Changer", this);
    auto_snapshots_path_layout->addWidget(auto_snapshots_path_button);

    connect(auto_snapshots_path_button, &QPushButton::clicked, this, [this, auto_snapshots_path] {
        QString path = QFileDialog::getExistingDirectory(this, tr("Changer le chemin de sauvegarde"),
                                                         QDir::homePath(),
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks);
        if (path.isEmpty())
            return;
        auto_snapshots_path->setText(path);
        _save_path = path;
    });

    _layout->addWidget(auto_snapshots);
}

void SettingsDialog::initAreaAlgorithmTypeUi() {
    auto AREA_ALGORITHM_TYPE = new QGroupBox("Algorithme de calcul de zone", this);

    auto AREA_ALGORITHM_TYPE_layout = new QVBoxLayout(AREA_ALGORITHM_TYPE);

    auto AREA_ALGORITHM_TYPE_combo_box = new QComboBox(this);
    AREA_ALGORITHM_TYPE_combo_box->addItems({"Colonnes", "Carrés"});
    AREA_ALGORITHM_TYPE_layout->addWidget(AREA_ALGORITHM_TYPE_combo_box);
    area_algorithm_type = Settings::AREA_ALGORITHM_TYPE;
    AREA_ALGORITHM_TYPE_combo_box->setCurrentIndex(static_cast<uint8_t >(area_algorithm_type));

    auto description = new QLabel(this);

    auto description_text = [this, description] {
        switch (area_algorithm_type) {
            case Settings::COLLUMNS: {
                if (Settings::DISPLAY_SIZE_WIDTH < Settings::NODES)
                    description->setText(
                            "Colonnes : Chaque noeud enfant calculera une colonne de pixels de l'image fractale.\nAttention, le nombre de colonnes est supérieur au nombre de noeuds, il est donc possible que certains noeuds ne calculent aucune colonne de pixels.");
                else
                    description->setText(
                            "Colonnes : Chaque noeud enfant calculera une colonne de pixels de l'image fractale");
            }
                break;
            case Settings::SQUARES: {
                if (Settings::NODES < 50)
                    description->setText(
                            "Carrés : Chaque noeud enfant calculera un carré de pixels de l'image fractale.\nAttention, le nombre de noeuds est faible pour ce mode de calcul, il est donc possible que certaines zones de l'image ne soient pas calculées.");
                else
                    description->setText(
                            "Carrés : Chaque noeud enfant calculera un carré de pixels de l'image fractale");
            }
                break;
        }
    };
    description_text();

    connect(AREA_ALGORITHM_TYPE_combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            [description_text, this](int index) {
                area_algorithm_type = static_cast<Settings::AreaAlgorithmType>(index);
                description_text();

            });

    AREA_ALGORITHM_TYPE_layout->addWidget(description);
    _layout->addWidget(AREA_ALGORITHM_TYPE);
}
