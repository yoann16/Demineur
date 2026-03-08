/* PROJET DEMINEUR YOANN JOSEPH
*/
// DEMINEUR ACHEVER.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
using namespace std;

/* BUG A CHANGER:
*
* Quand on tape une lettre plus un chiffre qui n'est pas sur le tableau
*
* TEST POUR VOIR LES AUTRES BUGS
*/

// Structure pour représenter un tableau 2D
struct Tableau2D
{
    char** tableau;         // Tableau caché avec bombes
    char** tableauVisuel;   // Tableau visible à l'utilisateur
    char** tableauSuperpose; // Tableau superposé
    int width;
    int height;
    int Bombe;

    // Constructeur pour initialiser le tableau 2D avec une taille donnée
    Tableau2D() : tableau(nullptr), tableauVisuel(nullptr), tableauSuperpose(nullptr), width(-1), height(-1), Bombe(1) {}

    // Destructeur pour libérer la mémoire allouée dynamiquement
    ~Tableau2D()
    {
        Destroy();
    }

    void Destroy()
    {
        for (int row = 0; row < height; ++row)
        {
            delete[] tableau[row];
            delete[] tableauVisuel[row];
            delete[] tableauSuperpose[row];
        }
        delete[] tableau;
        delete[] tableauVisuel;
        delete[] tableauSuperpose;
    }

    void create(int height_, int width_, int Bombe_)
    {
        width = width_;
        height = height_;
        Bombe = Bombe_;

        tableau = new char* [height];
        tableauVisuel = new char* [height];
        tableauSuperpose = new char* [height];

        for (int row = 0; row < height; ++row)
        {
            tableau[row] = new char[width];          // Tableau caché
            tableauVisuel[row] = new char[width];    // Tableau visible
            tableauSuperpose[row] = new char[width]; // Tableau superposé
            for (int col = 0; col < width; ++col)
            {
                tableau[row][col] = '.';             // Initialiser toutes les cellules à vide dans le tableau caché
                tableauVisuel[row][col] = '#';        // Initialiser toutes les cellules visibles à '#'
                tableauSuperpose[row][col] = '#';     // Initialiser toutes les cellules superposées à '#'
            }
        }

        // Spawn des bombes aléatoirement 
        int i = 0;
        while (i < Bombe)
        {
            int aleaRow = rand() % height;// prend un nombre aléatoire et le divise par height et prend le reste pour le mettre dans la variable aleaRow
            int aleaCol = rand() % width;// prend un nombre aléatoire et le divise par width et prend le reste pour le mettre dans la variable aleaCol

            if (tableau[aleaRow][aleaCol] != 'X') //Fait spawn toutes les bombes sans empilement
            {
                tableau[aleaRow][aleaCol] = 'X';
                ++i;
            }
        }
    }

    // Fonction pour afficher le tableau 2D avec des colonnes en lettres et lignes numérotées
    void afficher() const
    {
        cout << ""; // Décalage pour les numéros de colonne
        for (int col = 0; col < width; ++col)
        {
            char colonne = 'A' + col;
            if (col <= 0)
                cout << "     " << colonne << "  ";
            else
                cout << colonne << "  "; // Affichage des lettres de colonne
        }
        cout << endl;

        // Affichage du tableau superposé avec les valeurs et lignes numérotées
        for (int row = 0; row < height; ++row)
        {
            if (row < 9)
                cout << row + 1 << " ";
            else
                cout << row + 1;
            cout << "|" << " ";  // Numéro de ligne avec barre verticale
            for (int col = 0; col < width; ++col)
            {
                // Affichage stylisé
                if (tableauSuperpose[row][col] == '#') {
                    cout << " # "; // Case cachée
                }
                else if (tableauSuperpose[row][col] == 'X') {
                    cout << " X "; // Bombe
                }
                else {
                    cout << " " << tableauSuperpose[row][col] << " "; // Case découverte
                }
            }
            cout << "|" << endl; // Fin de la ligne
        }
    }

    // Fonction pour obtenir les indices de ligne et colonne à partir d'une référence comme "A16"
    // les const garantissent que la fonction et l'argument string ne modifiront pas les membres de la structures ou l'argument string pour cette fonction
    bool obtenirIndices(const string& reference, int& ligne, int& colonne) const
    {
        if (reference.length() < 2)
            return false;// La référence doit être au moins de la forme "A1", "B2", etc.

        char colChar = reference[0];//récupère le premier caractère de la chaîne de caractère( A,B,C...)

        // Extraire le reste de la référence pour obtenir le numéro de ligne:

        string rowString = reference.substr(1);// extrait le deuxième caractère jusqu'à la fin de la chaine de caractère( 1,10,120...)

        colonne = colChar - 'A'; // Convertir la colonne de lettre en un index (A=0,B=1,C=3...)
        ligne = stoi(rowString) - 1;// convertie la chaine de caractère rowString en un entier puis convertie l'index basé à 1 à 0

        // Vérifier si les indices obtenus sont dans les limites du tableau
        return (colonne >= 0 && colonne < width && ligne >= 0 && ligne < height);
    }


    // Fonction pour compter le nombre de bombes autour d'une cellule donnée
    int compterBombesAdjacentes(int ligne, int colonne)
    {
        int bombes = 0;

        // Parcourir les 8 cases autour de la cellule [ligne][colonne]
        for (int i = -1; i <= 1; ++i)
        {
            for (int j = -1; j <= 1; ++j)
            {

                int newRow = ligne + i;
                int newCol = colonne + j;

                //Vérifie si la cellule à coté est dans le tableau ou en dehors
                //Si il est à l'intérieur du tableau 
                //Alors on vérifie si la cellule à coté à une bombe  
                //et on incrémente sa valeur
                if (newRow >= 0 && newRow < height && newCol >= 0 && newCol < width)
                {
                    if (tableau[newRow][newCol] == 'X')
                    {
                        ++bombes;
                    }
                }
            }
        }
        return bombes;
    }

    // Fonction récursive pour découvrir les cellules adjacentes
    void decouvrirCellule(int ligne, int colonne)
    {
        if (ligne < 0 || ligne >= height || colonne < 0 || colonne >= width || tableauSuperpose[ligne][colonne] != '#')
            return; // Vérifier si la cellule est déjà révélée

        int bombesAdjacentes = compterBombesAdjacentes(ligne, colonne);

        if (bombesAdjacentes > 0)
        {
            tableauSuperpose[ligne][colonne] = '0' + bombesAdjacentes; // Afficher le nombre de bombes adjacentes
        }
        else
        {
            tableauSuperpose[ligne][colonne] = '.'; // Révéler une case vide

            // Révéler les cellules voisines
            for (int i = -1; i <= 1; ++i)
            {
                for (int j = -1; j <= 1; ++j)
                {
                    decouvrirCellule(ligne + i, colonne + j);
                }
            }
        }
    }

    //faire plus de commentaire:
    // Fonction pour gérer la commande de l'utilisateur
    void mettreAJourCellule(const string& reference, int command)
    {
        int ligne, colonne;
        if (obtenirIndices(reference, ligne, colonne))
        {
            if (command == 1)
            {
                if (tableau[ligne][colonne] == 'X')
                {
                    cout << "Vous avez cliquer sur une bombe ! Game Over." << endl;
                    // Afficher toutes les bombes
                    for (int r = 0; r < height; ++r)
                    {
                        for (int c = 0; c < width; ++c)
                        {
                            if (tableau[r][c] == 'X')
                                tableauSuperpose[r][c] = 'X'; // Révéler toutes les bombes
                        }
                    }
                    afficher(); // Afficher le tableau après la perte
                    exit(0);//Fini l'éxécution du programme
                }
                else
                {
                    decouvrirCellule(ligne, colonne); // Découvrir les cellules
                    //afficher();
                }
            }
            else if (command == 2)
            {
                tableauSuperpose[ligne][colonne] = 'F'; // Marquer la cellule avec un drapeau
                //afficher();
            }
        }
        else
        {
            cout << "Reference de cellule invalide." << endl;
            //afficher();
        }
        afficher();
    }
};
void afficherTitre()
{
    cout << "  __  __   ___   _   _   ________    _____   __          __   ________   ________    ____    ________    ____         \n";
    cout << " |  \\/  | |_ _| | \\ | | | _______|  / ____|  \\ \\        / /  | _______| | _______|  |  _ \\  | _______|  |  _ \\      \n";
    cout << " | \\  / |  | |  |  \\| | | |_____   | (___     \\ \\  /\\  / /   | |_____   | |_____    | |_) | | |_____    | |_) |      \n";
    cout << " | |\\/| |  | |  | |\\  | | ______|  \\___  \\     \\ \\/  \\/ /    | ______|  | ______|   |  __/  | ______|   |  _ <      \n";
    cout << " | |  | |  | |  | | \\ | | |______   ____) |     \\  /\\  /     | |______  | |______   | |     | |______   | | \\ \\    \n";
    cout << " |_|  |_| |___| |_| \\_| |________|  |_____/      \\/  \\/      |________| |________|  |_|     |________|  |_|  \\_\\      \n";
    cout << "-------------------------------------------------------------------------------------------------------------------  \n";
}

// Fonction principale
int main()
{
    ofstream myfile;

    int select;
    afficherTitre();
    cout << "\n\n";
    cout << "1 : Start Game" << endl;
    cout << "2 : Load Game" << std::endl;
    cout << "3 : Exit Game" << endl;
    cin >> select;

    if (select == 1)
    {
        Tableau2D Tableau2D;
        int Niv;
        cout << "Choisissez votre niveau de difficulte :" << endl;
        cout << "1 : Facile" << endl;
        cout << "2 : Normal" << endl;
        cout << "3 : Difficile" << endl;
        cout << "4 : Personallise" << std::endl;
        cin >> Niv;

        if (Niv == 1)
            Tableau2D.create(9, 9, 10);
        else if (Niv == 2)
            Tableau2D.create(16, 16, 40);
        else if (Niv == 3)
            Tableau2D.create(26, 16, 99);
        else if (Niv == 4)
        {
            int l;
            int h;
            int b;
            cout << "Longueur de la grille :";
            cin >> l;
            cout << "\n";
            cout << "Hauteur de la grille :";
            cin >> h;
            cout << "\n";
            cout << "Nombre de bombe de la grille :";
            cin >> b;
            cout << "\n";
            cout << "Il y a " << b << " bombe";
            cout << "\n\n";


            Tableau2D.create(h, l, b);
        }

        Tableau2D.afficher();

        while (true)
        {
            string reference;
            int choix;
            cout << "Voulez vous Sauvegarder ? " << endl;
            cout << "1 : OUI" << endl;
            cout << "2 : NON" << endl;
            cin >> choix;
            if (choix == 1)
            {
                ofstream myfile("Save.txt", ios::binary);

                myfile.write(reinterpret_cast<const char*>(&Tableau2D), sizeof(Tableau2D));
                // myfile.write(reinterpret_cast<const char*>(Tableau2D.tableauSuperpose), sizeof(Tableau2D.tableauSuperpose));
                // myfile.write(reinterpret_cast<const char*>(Tableau2D.tableauVisuel), sizeof(Tableau2D.tableauVisuel));
                cout << "Fichier Sauvegarder !" << endl;
            }
            cout << "\nEntrez une reference de cellule que vous voulez modifier (par exemple, B3) : ";
            cin >> reference;

            int command;
            cout << "1 : Fait un clique pour decouvrir la case" << endl;
            cout << "2 : Marque la case avec 'F'" << endl;
            cin >> command;

            Tableau2D.mettreAJourCellule(reference, command);
            //Tableau2D.afficher();

            // Vérifier si toutes les cellules non-bombes ont été découvertes
            bool gagne = true;
            for (int row = 0; row < Tableau2D.height; ++row)
            {
                for (int col = 0; col < Tableau2D.width; ++col)
                {
                    if (Tableau2D.tableau[row][col] != 'X' && Tableau2D.tableauSuperpose[row][col] == '#')
                    {
                        gagne = false; // Une cellule non-bombe reste cachée
                        break;
                    }
                }
                if (!gagne)
                    break; // Sortir si une cellule non-bombe est trouvée
            }

            if (gagne)
            {
                cout << "Felicitations ! Vous avez decouvert toutes les cellules non-bombes !" << endl;
                exit(0); // Terminer le jeu
            }
        }



    }
    /* else if (select == 2)
    {
        Tableau2D loadTableau2D;
        ifstream myfile("Save.txt", ios::binary);
        if (myfile.is_open())
        {
            myfile.read(reinterpret_cast<const char*>(&loadTableau2D), sizeof(loadTableau2D));
            myfile.close();
            loadTableau2D.tableau;
            loadTableau2D.tableauSuperpose;
            loadTableau2D.tableauVisuel;

            loadTableau2D.afficher();

            while (true)
            {
                string reference;
                int choix;
                cout << "Voulez vous Sauvegarder ? " << endl;
                cout << "1 : OUI" << endl;
                cout << "2 : NON" << endl;
                cin >> choix;
                if (choix == 1)
                {
                    ofstream myfile("Save.txt", ios::binary);

                    myfile.write(reinterpret_cast<const char*>(&loadTableau2D), sizeof(loadTableau2D));
                    // myfile.write(reinterpret_cast<const char*>(Tableau2D.tableauSuperpose), sizeof(Tableau2D.tableauSuperpose));
                    // myfile.write(reinterpret_cast<const char*>(Tableau2D.tableauVisuel), sizeof(Tableau2D.tableauVisuel));
                    cout << "Fichier Sauvegarder !" << endl;
                }
                cout << "\nEntrez une reference de cellule que vous voulez modifier (par exemple, B3) : ";
                cin >> reference;

                int command;
                cout << "1 : Fait un clique pour decouvrir la case" << endl;
                cout << "2 : Marque la case avec 'F'" << endl;
                cin >> command;

                loadTableau2D.mettreAJourCellule(reference, command);
                //Tableau2D.afficher();

                // Vérifier si toutes les cellules non-bombes ont été découvertes
                bool gagne = true;
                for (int row = 0; row < loadTableau2D.height; ++row)
                {
                    for (int col = 0; col < loadTableau2D.width; ++col)
                    {
                        if (loadTableau2D.tableau[row][col] != 'X' && loadTableau2D.tableauSuperpose[row][col] == '#')
                        {
                            gagne = false; // Une cellule non-bombe reste cachée
                            break;
                        }
                    }
                    if (!gagne)
                        break; // Sortir si une cellule non-bombe est trouvée
                }

                if (gagne)
                {
                    cout << "Felicitations ! Vous avez decouvert toutes les cellules non-bombes !" << endl;
                    exit(0); // Terminer le jeu
                }
            }


        }
        else
            cout << "Unable to open file";
    }*/

    return 0;
}