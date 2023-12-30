<h1 align="center">Fractalium</h1>

<p align="center">
  <img width="250px" height="250px" src="https://github.com/Sudo-Rahman/Fractalium/blob/main/documentation/image/mandelbrot1.png" alt="Mandelbrot" />
  <img width="250px" height="250px" src="https://github.com/Sudo-Rahman/Fractalium/blob/main/documentation/image/mandelbrot2.png" alt="Mandelbrot" />
  <img width="250px" height="250px" src="https://github.com/Sudo-Rahman/Fractalium/blob/main/documentation/image/julia1.png" alt="Julia" />
  <img width="250px" height="250px" src="https://github.com/Sudo-Rahman/Fractalium/blob/main/documentation/image/julia2.png" alt="Julia" />
  <img width="250px" height="250px" src="https://github.com/Sudo-Rahman/Fractalium/blob/main/documentation/image/bs1.png" alt="Burning Ship" />
  <img width="250px" height="250px" src="https://github.com/Sudo-Rahman/Fractalium/blob/main/documentation/image/bs2.png" alt="Burning Ship" />
</p>


# Calcul Distribué de l'Ensemble de Mandelbrot avec C++ MPI

## Aperçu

Ce projet implémente la génération de l'ensemble de Mandelbrot en utilisant
une approche de calcul distribué en C++ MPI. L'ensemble de Mandelbrot est un
fractal célèbre en mathématiques et cette implémentation permet le calcul parallèle de l'ensemble en utilisant une
architecture distribuée.

## Fonctionnalités

- **Calcul Distribué :** Utilise C++ MPI pour la répartition du calcul sur plusieurs nœuds.

- **Génération de l'Ensemble de Mandelbrot :** Calcule et visualise l'ensemble de Mandelbrot en parallèle.

- **Paramètres Configurables :** Configurez facilement la résolution, le niveau de zoom et d'autres paramètres pour
  explorer différentes parties de l'ensemble de Mandelbrot.

## Prérequis

- C++
- Boost
- Boost MPI
- MPI
- Git
- Qt >=5

## Démarrage

1. **Cloner le Dépôt :**

```bash
git clone https://github.com/Sudo-Rahman/Fractalium.git
```

2. **Compilation :**

```bash
mkdir build
```

```bash
cd build
```

```bash
cmake ..
```

```bash
make
```

3. **Exécution :**

Pour éxecuter en local :

```bash
mpirun -np nombre_de_threads ./Fractalium
```

Pour éxecuter sur un cluster :

```bash
mpirun -hostfile -hostfile fichier_hosts ./Fractalium
```






