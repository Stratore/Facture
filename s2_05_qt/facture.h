#ifndef FACTURE_H
#define FACTURE_H
#include <QString>

class Facture
{
public:
    Facture(double indexInitial = 0, double indexActuel = 0);
    void setReleves(double initial, double actuel);
    double getConsommation() const;      // Différence des index
    double getCoutConsommation() const;  // Consommation * Prix du kWh
    double getMontantTotalTTC() const;   // Cout consommation + Abonnement
    bool estValide() const;
    QString getResume() const;           // Génère le texte détaillé pour l'affichage

    // Méthodes statiques pour l'admin
    static double getPrixKwh();
    static void setPrixKwh(double prix);
    static double getAbonnement();
    static void setAbonnement(double abo);

private:
    double m_indexInitial;
    double m_indexActuel;

    // Variables tarifaires modifiables par l'admin
    static double m_abonnementMensuel;
    static double m_prixkWh;
};

#endif // FACTURE_H
