#ifndef FACTURE_H
#define FACTURE_H
#include <QString>

class Facture
{
public:

    Facture(double indexInitial = 0, double indexActuel = 0);

    // je met a jour les index de la facture
    void setReleves(double initial, double actuel);

    double getConsommation() const;
    double getCoutConsommation() const;
    double getMontantTotalTTC() const;

    // je verifie si les valeurs sont logiques
    bool estValide() const;
    QString getResume() const;

    // parti admin par julyan
    static void setPrixKwh(double prix);
    static void setAbonnement(double abo);

    static double getPrixKwh();
    static double getAbonnement();

private :
    double m_indexInitial;
    double m_indexActuel;

    static double m_abonnementMensuel;
    static double m_prixkWh;
};

#endif // FACTURE_H
