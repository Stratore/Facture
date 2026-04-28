#include "facture.h"

// initialisation des tarifs (partie julyan)
double Facture::m_abonnementMensuel = 12.60;
double Facture::m_prixkWh = 0.2516;

Facture::Facture(double indexInitial, double indexActuel)
    : m_indexInitial(indexInitial), m_indexActuel(indexActuel) {}

void Facture::setReleves(double initial, double actuel) {
    m_indexInitial = initial;
    m_indexActuel = actuel;
}

bool Facture::estValide() const {
    // le nouvel index doit etre > que l ancien
    return (m_indexActuel >= m_indexInitial);
}

double Facture::getConsommation() const {
    // calcul de la conso
    return (estValide()) ? (m_indexActuel - m_indexInitial) : 0;
}

double Facture::getCoutConsommation() const {
    return getConsommation() * m_prixkWh;
}

double Facture::getMontantTotalTTC() const {
    return getCoutConsommation() + m_abonnementMensuel;
}

QString Facture::getResume() const {
    if (!estValide()) return "erreur : index incoherents.";
    return QString("conso: %1 kwh | total: %2 euros")
        .arg(getConsommation())
        .arg(getMontantTotalTTC(), 0, 'f', 2);
}

// getters et setters des stats
void Facture::setPrixKwh(double prix) { m_prixkWh = prix; }
void Facture::setAbonnement(double abo) { m_abonnementMensuel = abo; }
double Facture::getPrixKwh() { return m_prixkWh; }
double Facture::getAbonnement() { return m_abonnementMensuel; }
