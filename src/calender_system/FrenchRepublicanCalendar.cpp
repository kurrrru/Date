#include <calender_system/FrenchRepublicanCalendar.hpp>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstring>

#include <string.hpp>

namespace {
bool is_leap(int year);
int last_day_of_month(int year, int month);
std::string to_string_Ee(int era, bool uppercase);
std::string to_string_Yy(int year, bool uppercase);
std::string to_string_Mm(int month, bool uppercase);
std::string to_string_Dd(int day, bool uppercase);
std::string to_string_Ww(int day_of_week, bool uppercase);

std::string get_month_name(int month, bool uppercase);
std::string get_day_name(int month, int day, bool uppercase);

}  // namespace

namespace toolbox {
FrenchRepublicanCalendar::FrenchRepublicanCalendar() {
}

FrenchRepublicanCalendar::FrenchRepublicanCalendar(
    const FrenchRepublicanCalendar& other) {
    (void)other;
}

FrenchRepublicanCalendar& FrenchRepublicanCalendar::operator=(
    const FrenchRepublicanCalendar& other) {
    (void)other;
    return *this;
}

FrenchRepublicanCalendar::~FrenchRepublicanCalendar() {
}


}  // namespace toolbox

namespace {
bool is_leap(int year) {
    // Leap year rule is very complex, here we use a simplified rule
    // which is not historically accurate.
    return year % 4 == 3;  // Leap year every 4 years
}

int last_day_of_month(int year, int month) {
    if (month < 1 || month > 13) {
        throw std::out_of_range("last_day_of_month failed: "
            "month must be in 1..13");
    }
    if (month == 13) {
        return is_leap(year) ? 6 : 5;
    }
    return 30;
}

// This implementation uses only ASCII characters.
// In a real implementation, accented characters should be used.
std::string get_month_name(int month) {
    const char* month_names[] = {
        /* 1  = */ "Vendemiaire",
        /* 2  = */ "Brumaire",
        /* 3  = */ "Frimaire",
        /* 4  = */ "Nivose",
        /* 5  = */ "Pluviose",
        /* 6  = */ "Ventose",
        /* 7  = */ "Germinal",
        /* 8  = */ "Floreal",
        /* 9  = */ "Prairial",
        /* 10 = */ "Messidor",
        /* 11 = */ "Thermidor",
        /* 12 = */ "Fructidor",
        /* 13 = */ "Sansculottides",
    };
    if (month < 1 || month > 13) {
        throw std::out_of_range("get_month_name failed: "
            "month must be in 1..13");
    }
    return month_names[month - 1];
}

std::string get_day_name(int month, int day) {
    if (month < 1 || month > 13) {
        throw std::out_of_range("get_day_name failed: "
            "month must be in 1..13");
    }
    if (day < 1 || (month == 13 && day > 6) || (month != 13 && day > 30)) {
        throw std::out_of_range("get_day_name failed: "
            "day is out of range for month " + toolbox::to_string(month));
    }
    int doy = (month - 1) * 30 + day;
    const char* day_names[] = {
        /* Vendemiaire (1) */
        "Raisin", "Safran", "Chataigne", "Colchique", "Cheval",
        "Balsamine", "Carotte", "Amaranthe", "Panais", "Cuve",
        "Pomme de terre", "Immortelle", "Potiron", "Reseda", "Ane",
        "Belle de nuit", "Citrouille", "Sarrasin", "Tournesol", "Pressoir",
        "Chanvre", "Peche", "Navet", "Amarillis", "Boeuf",
        "Aubergine", "Piment", "Tomate", "Orge", "Tonneau",

        /* Brumaire (2) */
        "Pomme", "Celeri", "Poire", "Betterave", "Oie",
        "Heliotrope", "Figue", "Scorsonere", "Alisier", "Charrue",
        "Salsifis", "Macre", "Topinambour", "Endive", "Dindon",
        "Chervis", "Cresson", "Dentelaire", "Grebade", "Herse",
        "Bacchante", "Azerole", "Garance", "Orange", "Faisan",
        "Pistache", "Mahjonc", "Coing", "Cormier", "Rouleau",

        /* Frimaire (3) */
        "Raiponce", "Turneps", "Chicoree", "Nefle", "Cochon",
        "Mache", "Chou-fleur", "Miel", "Genièvre", "Pioche",
        "Cire", "Raifort", "Cedre", "Sapin", "Chevreuil",
        "Ajonc", "Cypres", "Lierre", "Sabine", "Hoyau",
        "Erable sucre", "Bruyere", "Roseau", "Oseille", "Grillon",
        "Pignon", "Liege", "Truffe", "Olive", "Pelle",

        /* Nivose (4) */
        "Tourbe", "Houille", "Bitume", "Soufre", "Chien",
        "Lave", "Terre vegetale", "Fumier", "Salpetre", "Fleau",
        "Granit", "Argile", "Ardoise", "Gres", "Lapin",
        "Silex", "Marne", "Pierre a chaux", "Marbre", "Van",
        "Pierre a platre", "Sel", "Fer", "Cuivre", "Chat",
        "Etain", "Plomb", "Zinc", "Mercure", "Crible",

        /* Pluviose (5) */
        "Laureole", "Mousse", "Fragon", "Perce-neige", "Taureau",
        "Laurier-tin", "Amadouvier", "Mezereon", "Peuplier", "Coignee",
        "Ellebore", "Brocoli", "Laurier", "Avelinier", "Vache",
        "Buis", "Lichen", "If", "Pulmonaire", "Serpette",
        "Thlaspi", "Thimele", "Chiendent", "Trainel", "Lievre",
        "Guede", "Noisetier", "Cyclamen", "Chelidoine", "Traineau",

        /* Ventose (6) */
        "Tussilage", "Cornouiller", "Violier", "Troene", "Bouc",
        "Asaret", "Alaterne", "Violette", "Marceau", "Beche",
        "Narcisse", "Orme", "Fumeterre", "Velar", "Chevre",
        "Epinard", "Doronic", "Mouron", "Cerfeuil", "Cordeau",
        "Mandragore", "Persil", "Cochlearia", "Paquerette", "Thon",
        "Pissenlit", "Sylvie", "Capillaire", "Frene", "Plantoir",

        /* Germinal (7) */
        "Primevere", "Platane", "Asperge", "Tulipe", "Poule",
        "Bette", "Bouleau", "Jonquille", "Aulne", "Greffoir",
        "Pervenche", "Charme", "Morille", "Hetre", "Abeille",
        "Laitue", "Meleze", "Cigue", "Radis", "Ruche",
        "Gainier", "Romaine", "Marronnier", "Roquette", "Pigeon",
        "Lilas", "Anemone", "Pensee", "Myrtille", "Couvoir",

        /* Floreal (8) */
        "Rose", "Chene", "Fougere", "Aubepine", "Rossignol",
        "Ancolie", "Muguet", "Champignon", "Hyacinthe", "Rateau",
        "Rhubarbe", "Sainfoin", "Baton-d'or", "Chamerops", "Ver a soie",
        "Consoude", "Pimprenelle", "Corbeille d'or", "Arroche", "Sarcloir",
        "Statice", "Fritillaire", "Bourrache", "Valeriane", "Carpe",
        "Fusain", "Civette", "Buglosse", "Seneve", "Houlette",

        /* Prairial (9) */
        "Luzerne", "Hemerocalle", "Trefle", "Angelique", "Canard",
        "Melisse", "Fromental", "Martagon", "Serpolet", "Faux",
        "Fraise", "Betoine", "Pois", "Acacia", "Caille",
        "Oeillet", "Sureau", "Pavot", "Tilleul", "Fourche",
        "Barbeau", "Camomille", "Chevrefeuille", "Caille-lait", "Tanche",
        "Jasmin", "Verveine", "Thym", "Pivoine", "Chariot",

        /* Messidor (10) */
        "Seigle", "Avoine", "Oignon", "Veronique", "Mulet",
        "Romarin", "Concombre", "Echalote", "Absinthe", "Faucille",
        "Coriandre", "Artichaut", "Girofle", "Lavande", "Chamois",
        "Tabac", "Groseille", "Gesse", "Cerise", "Parc",
        "Menthe", "Cumin", "Haricot", "Orcanete", "Dindette",
        "Sauge", "Ail", "Vesce", "Ble", "Chalemie",

        /* Thermidor (11) */
        "Epeautre", "Bouillon-blanc", "Melon", "Ivraie", "Belier",
        "Prele", "Armoise", "Carthame", "Mure", "Arrosoir",
        "Panis", "Salicorne", "Abricot", "Basilic", "Brebis",
        "Guimauve", "Lin", "Amande", "Gentiane", "Ecluse",
        "Carline", "Caprier", "Lentille", "Aunee", "Loutre",
        "Myrte", "Colza", "Lupin", "Coton", "Moulin",

        /* Fructidor (12) */
        "Prune", "Millet", "Lycoperdon", "Escourgeon", "Saumon",
        "Tubereuse", "Sucrion", "Apocyn", "Reglisse", "Echelle",
        "Pasteque", "Fenouil", "Epine vinette", "Noix", "Truite",
        "Citron", "Cardere", "Nerprun", "Tagette", "Hotte",
        "Eglantier", "Noisette", "Houblon", "Sorgho", "Ecrevisse",
        "Bigarade", "Verge d'or", "Mais", "Marron", "Panier",

        /* Jours complementaires (13) */
        "Jour de la Vertu",
        "Jour du Genie",
        "Jour du Travail",
        "Jour de l'Opinion",
        "Jour des Recompenses",
        "Jour de la Revolution"
    };
    return day_names[doy - 1];
}

}  // namespace
