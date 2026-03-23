#include "mainwindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFont>
#include <QMenuBar>
#include <QFileDialog>


bool MainWindow::verifconex() {     //cette fonctione permet de vérifier si l'utilisateur a marqué qqc avant de faire suivant
    if (!champID->text().isEmpty() && !champMDP->text().isEmpty())
        return true;
    else
        return false;
}


void MainWindow::LayoutSuivant() {

    if (verifconex()) {

        QWidget *conteneurAccueil = new QWidget(); //on crée un nouveau conteneur

        layoutAccueil = new QVBoxLayout(conteneurAccueil); // on crée un nouveau layout

    // Ajoutons un petit texte pour vérifier que ça marche

        QLabel *bienvenue = new QLabel("Bienvenue dans votre session !");
        bienvenue->setAlignment(Qt::AlignCenter);
        layoutAccueil->addWidget(bienvenue);

        setCentralWidget(conteneurAccueil);

    } else {

        champID->clear();
        champMDP->clear();
        champID->setPlaceholderText("veuillez remplir les deux champs");
    }

}



void MainWindow::ouvrir() {  //methode ouvrire
    QString fileName = QFileDialog::getOpenFileName(
        0,
        "Ouvrir un fichier",
        "/",
        "Fichiers textes (*.txt);;Fichiers XML (*.xml)");

    if( fileName != "" )
    {
        QFile file(fileName);
        if( file.open(QIODevice::Text | QIODevice::ReadOnly) )
        {
            QTextStream stream(&file);
            layoutAccueil->setText(stream.readAll());
            file.close();
        }
    }
    setWindowTitle(fileName);
}







MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //paramètre de la fenêtre
    setWindowTitle("Facture");  //nom de la fenêtre
    setWindowFlags(Qt::WindowStaysOnTopHint);       //pour le fait que quand elle aparaisse elle soit au dessu de tout
    resize(700,700);        //la taille de la fenêtre

    //def menu :
    // 1. On crée un widget "conteneur"
    QWidget *conteneur = new QWidget(this);

    // 2. On dit à la MainWindow que c'est lui le centre
    setCentralWidget(conteneur);

    // 3. On crée le layout EN LE RATTACHANT au conteneur
    MenuConnecter = new QVBoxLayout(conteneur);






    //Le TITRE
    QLabel *label = new QLabel();                       //titre s'identifer
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setText("s'identifier");
    label->setAlignment(Qt::AlignTop | Qt::AlignCenter);

    //la police
    QFont police;
    police.setFamily("Segoe UI");
    police.setPointSize(26);
    police.setBold(true);
    label->setFont(police);
    MenuConnecter -> addWidget(label);    //Le TITRE


    MenuConnecter->addSpacing(30);      //pour rajouter un espace


    //style pour les champs
    QString styleChamps =
        "QLineEdit {"
        "  border: 1px solid #dbdbdb;"
        "  border-radius: 5px;"
        "  padding: 8px;"              // Espace le texte du bord
        "  background-color: #fafafa;"
        "  color: #262626;"
        "}"
        "QLineEdit:focus {"            // Quand on clique dedans
        "  border: 1px solid #a8a8a8;"
        "}";


    //Champ ID
    champID = new QLineEdit();       // création de la case ou on mettra le ID
    champID->setEchoMode(QLineEdit::Normal);  //ca permet de mettre le champ nprmal
    champID->setPlaceholderText("Nom Utilisateur");
    MenuConnecter -> addWidget(champID);
    champID->setFocus();
    champID->setStyleSheet(styleChamps);





    MenuConnecter->addSpacing(15);



    //Champ MDP
    champMDP = new QLineEdit();       // création de la case ou on mettra la MDP
    champMDP->setEchoMode(QLineEdit::Password);  //ca permet de mettre le mot de passe en ******** quand on l'écrit pour la confidentialité
    champMDP->setPlaceholderText("Mot de passe");
    MenuConnecter -> addWidget(champMDP);
    champMDP->setStyleSheet(styleChamps);

    MenuConnecter->addSpacing(30);





    //Bouton suivant
    suivant = new QPushButton("suivant", this);
    connect(suivant, SIGNAL(clicked()), this, SLOT(LayoutSuivant()));      //quand on clique dessu ca fais la méthode suivant(

    suivant->setStyleSheet(
        "QPushButton {"
        "  background-color: #0095f6;" // Bleu Instagram
        "  color: white;"               // Texte blanc
        "  border-radius: 8px;"        // Bords arrondis
        "  font-weight: bold;"
        "  border: none;"
        "}"
        "QPushButton:hover {"          // Quand on passe la souris dessus
        "  background-color: #1877f2;"
        "}"
        "QPushButton:pressed {"        // Quand on clique
        "  background-color: #0074cc;"
        "}"
        );

    MenuConnecter -> addWidget(suivant);
    suivant->setFixedWidth(120);
    suivant->setMinimumHeight(35);

    //Création d'un layout en H pour suivant pour le metre plus a droite
    QHBoxLayout *layoutBouton = new QHBoxLayout();
    //layoutBouton->addStretch(); // Le ressort pousse le bouton vers la droite
    layoutBouton->addWidget(suivant); // On ajoute le bouton
    MenuConnecter->addLayout(layoutBouton);


    MenuConnecter->addStretch();        // pour enlever l'espace entre s'identifier et le reste




    //Menu fichier ---------------------------------------------------------
    QMenu *MenuFichier = new QMenu(QString::fromUtf8("&Fichier"), this);
    menuBar()->addMenu(MenuFichier);  //création de fichier


    //crée l'action ouvrire ------
    QAction *actionOuvrir = new QAction(QString::fromUtf8("Ouvrir"), this);

    connect(actionOuvrir, SIGNAL(triggered()), this, SLOT(ouvrir()));
    //ajouter action enregistrer au menu nouveau
    MenuFichier->addAction(actionOuvrir);


    affichageFacture = new QTextEdit();
    affichageFacture->setReadOnly(true); // On ne veut pas que l'utilisateur modifie la facture
    layoutAccueil->addWidget(affichageFacture);

    QPushButton *btnOuvrir = new QPushButton("Charger une facture");
    connect(btnOuvrir, &QPushButton::clicked, this, &MainWindow::ouvrir);
    layoutAccueil->addWidget(btnOuvrir);

    setCentralWidget(conteneurAccueil);
}

}



MainWindow::~MainWindow() {}
