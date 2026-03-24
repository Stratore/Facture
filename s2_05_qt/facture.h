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

private :
    // Données de saisie
    double m_indexInitial;
    double m_indexActuel;

    // Variables tarifaires l'admine aura accèes a ces donnée pour les changer
    const double m_abonnementMensuel = 12.60;
    const double m_prixkWh = 0.2516;
};

#endif // FACTURE_H
