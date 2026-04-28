#include "facture.h"

// Initialisation des tarifs (Partie Julyan)
double Facture::m_abonnementMensuel = 12.60;
double Facture::m_prixkWh = 0.2516;

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
    return (estValide()) ? (m_indexActuel - m_indexInitial) : 0;
}

double Facture::getCoutConsommation() const {
    return getConsommation() * m_prixkWh;
}

double Facture::getMontantTotalTTC() const {
    return getCoutConsommation() + m_abonnementMensuel;
}

QString Facture::getResume() const {
    if (!estValide()) return "Erreur : Index incohérents.";
    return QString("Conso: %1 kWh | Total: %2 €")
        .arg(getConsommation())
        .arg(getMontantTotalTTC(), 0, 'f', 2);
}

// Getters et Setters statiques
void Facture::setPrixKwh(double prix) { m_prixkWh = prix; }
void Facture::setAbonnement(double abo) { m_abonnementMensuel = abo; }
double Facture::getPrixKwh() { return m_prixkWh; }
double Facture::getAbonnement() { return m_abonnementMensuel; }
