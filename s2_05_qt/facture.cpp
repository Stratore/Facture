#include "facture.h"

Facture::Facture(double indexInitial, double indexActuel)
    : m_indexInitial(indexInitial), m_indexActuel(indexActuel) {}

void Facture::setReleves(double initial, double actuel) {
    m_indexInitial = initial;
    m_indexActuel = actuel;
}

bool Facture::estValide() const {

    return (m_indexActuel >= m_indexInitial);
}

double Facture::getConsommation() const {
    if (!estValide()) return 0;
    return m_indexActuel - m_indexInitial;
}

double Facture::getCoutConsommation() const {
    return getConsommation() * m_prixkWh;
}

double Facture::getMontantTotalTTC() const {
    return getCoutConsommation() + m_abonnementMensuel;
}

QString Facture::getResume() const {
    if (!estValide()) {
        return "Erreur : L'index actuel est inférieur à l'index initial.";
    }

    return QString(
               "--- RÉCAPITULATIF DE FACTURE ---\n"
               "Consommation totale : %1 kWh\n"
               "--------------------------------\n"
               "Abonnement fixe : %2 €\n"
               "Coût consommation : %3 €\n"
               "--------------------------------\n"
               "MONTANT TOTAL TTC : %4 €"
               )
        .arg(getConsommation())
        .arg(m_abonnementMensuel, 0, 'f', 2)
        .arg(getCoutConsommation(), 0, 'f', 2)
        .arg(getMontantTotalTTC(), 0, 'f', 2);
}
