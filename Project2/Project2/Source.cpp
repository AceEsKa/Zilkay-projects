/*
Meno a priezvisko:
POKYNY:
(1)  Subor premenujte na Priezvisko_Meno_ID_zadanie02.cpp (pouzite vase udaje bez diakritiky).
(2)  Implementujte funkcie tak, aby splnali popis pri ich deklaraciach.
(3)  Cela implementacia musi byt v tomto jednom subore.
(4)  Odovzdajte len tento (spravne premenovany) zdrojovy subor.
(5)  Program musi byt kompilovatelny.
(6)  Globalne a staticke premenne su zakazane.
(7)  V ziadnom pripade nemente deklaracie funkcii, ktore mate za ulohu naprogramovat
     (nemente nazvy, navratove hodnoty, ani typ a pocet parametrov v zadanych funkciach).
     Nemente implementacie zadanych datovych typov, ani implementacie hotovych pomocnych funkcii
     (ak nie je v zadani ulohy uvedene inak).
(8)  V pripade potreby mozete kod doplnit o dalsie pomocne funkcie alebo datove typy.
(9)  Vase riesenie otestujte (vo funkcii 'main' a pomocou doplnenych pomocnych funkcii alebo datovych typov).
     Testovaci kod ale nebude hodnoteny.
(10) Funkcia 'main' musi byt v zdrojovom kode posledna.
*/


#include <iostream>
#include <memory>
#include <stack>
using namespace std;

//-------------------------------------------------------------------------------------------------
// DATOVE TYPY
//-------------------------------------------------------------------------------------------------

// Reprezentacia uzla zretazeneho zoznamu
struct Node {
    int data; // hodnota uzla
    Node* next; // adresa nasledujuceho uzla zoznamu
};

// Reprezentacia zretazeneho zoznamu
struct List {
    Node* first; // adresa prveho uzla zoznamu
};

// Pomocna struktura pre ulohu 2
struct ListData {
    int* data; // pole hodnot uzlov
    size_t len; // dlzka pola 'data'
};

Node* newNode(const int value)
{
    Node* n_node = new Node();
    n_node->data = value;
    n_node->next = nullptr;

    return n_node;
}

//-------------------------------------------------------------------------------------------------
// 1. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia prida novy uzol s hodnotou 'val' na koniec zretazeneho zoznamu 'list'.

    VSTUPNE PODMIENKY:
        1) Plati, ze 'list' vzdy existuje (t.j. 'list' je rozny od 'nullptr') a moze
           mat lubovolny pocet uzlov (0 a viac).

    PARAMETRE:
        [in/out] list   - adresa zretazeneho zoznamu, do ktoreho ideme pridavat uzol
        [in] val        - hodnota uzla, ktory sa ma pridat na koniec zoznamu 'list'

    NAVRATOVA HODNOTA:
        Ziadna.

    PRIKLADY:
        {prazdny zoznam},5 ---> {5}
        {1},1 ---> {1,1}
        {8,2,6,6},14 ---> {8,2,6,6,14}
        {6,100,33},-85 ---> {6,100,33,-85}
        {1,2,3,4,5},3 ---> {1,2,3,4,5,3}
*/

void appendNode(List* list, const int val) {
    // TODO
    Node* premenna = newNode(val);
    Node* tmp = list->first;
    if (list->first == nullptr)
    {
        list->first = premenna;
    }
    else
    {
        tmp = list->first;
        while (tmp->next != nullptr)
        {
            tmp = tmp->next;
        }
        tmp->next = premenna;
    }
}


//-------------------------------------------------------------------------------------------------
// 2. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia vytvori novy zretazeny zoznam, ktory bude skonstruovany pomocou udajov vo vstupnej
    strukture 'list_data'. Struktura 'list_data' obsahuje polozku 'data', ktora reprezentuje pole
    hodnot pre jednotlive uzly zoznamu. Struktura 'list_data' taktiez obsahuje polozku 'len', ktora
    hovori aku ma dlzku pole 'data'. Vytvoreny zoznam musi obsahovat uzly, ktore zodpovedaju
    svojimi hodnotami a poradim prvkom ulozenym v poli 'data' (dlzka zoznamu bude 'len').
    Funkcia nakoniec vrati adresu takto vytvoreneho zoznamu.

    VSTUPNE PODMIENKY:
        1) Plati, ze 'list_data' vzdy existuje (t.j. 'list_data' je rozny od 'nullptr').
        2) Pre polozku 'len' plati, ze 'len' >= 0 a nikdy nebude mat vacsiu hodnotu ako je
           realny pocet prvkov v poli 'data'.
        3) Ak je polozka 'len' rovna 0, vtedy sa vytvori a vrati prazdny zretazeny zoznam
           (t.j. platny zoznam, ktory ma 0 uzlov).

    PARAMETRE:
        [in] list_data - struktura, pomocou ktorej sa vytvori zoznam

    NAVRATOVA HODNOTA:
        Adresa zretazeneho zoznamu, ktory bol vytvoreny pomocou struktury 'list_data'.
        Vid pokyny vyssie.

    PRIKLADY:
        polozka 'data' = [1,2,3]
        polozka 'len' = 0
        vystupny zoznam bude mat 0 uzlov, t.j. {}

        polozka 'data' = [1,2,3]
        polozka 'len' = 2
        vystupny zoznam bude {1,2}

        polozka 'data' = [1,2,3]
        polozka 'len' = 3
        vystupny zoznam bude {1,2,3}

        polozka 'data' = [-9,412,64,72,100,100,6]
        polozka 'len' = 4
        vystupny zoznam bude {-9,412,64,72}

*/

List* createList(const ListData* list_data) {
    // TODO

    List* c_list = new List();
    c_list->first = nullptr;

    for (int i = 0; i < list_data->len; i++)
    {
        appendNode(c_list, *(list_data->data + i));
    }

    return c_list; // tento riadok zmente podla zadania, je tu len kvoli kompilacii
}


//-------------------------------------------------------------------------------------------------
// 3. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia prida novy uzol s hodnotou 'val' do vzostupne zotriedeneho zoznamu 'sorted_list'.
    Pridanie musi prebehnut takym sposobom, aby bol zoznam 'sorted_list' po pridani noveho uzla
    stale vzostupne zotriedeny.

    POZNAMKA: Na poradi duplikatov v zozname 'sorted_list' nezalezi.

    VSTUPNE PODMIENKY:
        1) Plati, ze 'sorted_list' vzdy existuje (t.j. 'sorted_list' je rozny od 'nullptr').
        2) Vstupny zoznam 'sorted_list' moze mat lubovolny pocet uzlov (0 a viac).
        3) Vstupny zoznam 'sorted_list' je vzostupne zotriedeny.

    PARAMETRE:
        [in/out] sorted_list    - adresa vstupneho zretazeneho zoznamu, ktory je zotriedeny
                                  vzostupne (z angl. ascending)
        [in] val                - hodnota uzla, ktory sa ma pridat do zoznamu 'sorted_list'

    NAVRATOVA HODNOTA:
        Ziadna.

    PRIKLADY:
        {},0 ---> {0}
        {},156 ---> {156}
        {8},4 ---> {4,8}
        {7},10 ---> {7,10}
        {55},55 ---> {55,55} ... poznamka: na poradi duplikatov nezalezi
        {1,8,236,888},70 ---> {1,8,70,236,888}
        {6,6,14,82,82,100},6 ---> {6,6,6,14,82,82,100}

*/

void insertNode(List* sorted_list, const int val) {
    // TODO
    //quick sort
    if (sorted_list->first == nullptr)
    {
        appendNode(sorted_list,val);
        return;
    }
    Node* tmp = sorted_list->first;
    Node* prev = sorted_list->first;
    Node* to_insert= newNode(val);
    //new head
    if (sorted_list->first->data >= val)
    {
        sorted_list->first = to_insert;
        to_insert->next = tmp;
        return;
    }

    while (tmp->next != nullptr)
    {
        if (val <= tmp->data)
        {
            prev->next = to_insert;
            to_insert->next = tmp;
            return;
        }
        prev = tmp;
        tmp = tmp->next;
    }
    tmp->next = to_insert;
}


//-------------------------------------------------------------------------------------------------
// 4. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia vytvori novy zoznam spojenim zoznamov 'list1' a 'list2'. Funkcia vrati adresu
    novo vytvoreneho zoznamu. V spojenom zozname idu najprv uzly zoznamu 'list1' (v nezmenenom poradi)
    a potom nasleduju uzly zoznamu 'list2' (v nezmenenom poradi). Pri vytvarani noveho zoznamu mozete
    pouzit hlboku alebo plytku kopiu existujucich uzlov (obe riesenia budu akceptovane).

    POZNAMKA:
        Prazdny zoznam je taky zoznam, ktory je platny, ale obsahuje 0 uzlov
        (t.j. polozka 'first' je nastavena na 'nullptr').

    VSTUPNE PODMIENKY:
        1) Zoznamy 'list1' a 'list2' mozu byt lubovolne dlhe (0 a viac uzlov).

    PARAMETRE:
        [in/out] list1 - adresa prveho zoznamu
        [in/out] list2 - adresa druheho zoznamu

    NAVRATOVA HODNOTA:
        Adresa zoznamu, ktory vznikol spojenim zoznamov 'list1' a 'list2'.

    PRIKLADY:
        {} + {} ---> {}
        {5,9} + {} ---> {5,9}
        {} + {2,2,2} ---> {2,2,2}
        {1} + {2} ---> {1,2}
        {7} + {3,9,16} ---> {7,3,9,16}
        {50,-47,23,1} + {0,0,9,-8} ---> {50,-47,23,1,0,0,9,-8}
*/

List* joinLists(List* list1, List* list2) {
    // TODO
    List* n_list = new List();
    if (list1->first == nullptr && list2->first == nullptr)
    {
        n_list->first = nullptr;
        return n_list;
    }
    else if (list1->first == nullptr)
    {
        n_list = list2;
        return n_list;
    }
    else if (list2->first == nullptr)
    {
        n_list = list1;
        return n_list;
    }

    n_list->first = list1->first;

    Node* tmp = n_list->first;
    while (tmp->next != nullptr)
    {
        tmp = tmp->next;
    }
    tmp->next = list2->first;

    return n_list; // tento riadok zmente podla zadania, je tu len kvoli kompilacii
}


//-------------------------------------------------------------------------------------------------
// 5. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia odstrani posledny uzol zo zoznamu 'list'. Ak je zoznam 'list' prazdny, funkcia nic
    nevykona. Pamat uzla, ktory funkcia vymaze musi byt korektne dealokovana.

    VSTUPNE PODMIENKY:
        1) Plati, ze 'list' vzdy existuje (t.j. 'list' je rozny od 'nullptr').
        2) Zoznam 'list' moze byt lubovolne dlhy (0 a viac uzlov).

    PARAMETRE:
        [in/out] list - adresa vstupneho zoznamu

    NAVRATOVA HODNOTA:
        Ziadna.

    PRIKLADY:
        {} ---> {}
        {1} ---> {}
        {1,2} ---> {1}
        {1,2,3} ---> {1,2}
*/

void removeLastNode(List* list) {
    // TODO
    if (list->first != nullptr)
    {
        Node* tmp = list->first;
        if (tmp->next == nullptr)
        {
            delete tmp;
            list->first = nullptr;
            return;
        }
        Node* prev = nullptr;
        while (tmp->next != nullptr)
        {
            prev = tmp;
            tmp = tmp->next;
        }
        delete tmp;
        tmp = nullptr;
        if (prev != nullptr)
        {
            prev->next = nullptr;
        }
    }
}


//-------------------------------------------------------------------------------------------------
// 6. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia overi, ci je vstupny zoznam 'list' palindrom. Prazdny zoznam budeme povazovat za
    palindrom.

    Poznamka: Palindrom predstavuje taku postupnost hodnot, ktora sa cita rovnako v oboch smeroch.
    URL: https://sk.wikipedia.org/wiki/Palindr%C3%B3m
    Priklad palindromu: {1212121} alebo {4953594}

    VSTUPNE PODMIENKY:
        1) Plati, ze 'list' vzdy existuje (t.j. 'list' je rozny od 'nullptr').
        2) Zoznam 'list' moze byt lubovolne dlhy (0 a viac uzlov).

    PARAMETRE:
        [in] list - adresa vstupneho zoznamu

    NAVRATOVA HODNOTA:
        Pravdivostna hodnota 'true'/'false' podla toho, ci vstupny zoznam je/nie je palindrom.

    PRIKLADY:
        {} ---> true
        {1} ---> true
        {2,2,2,2} ---> true
        {1,2,3,4,5} ---> false
        {1,2,2,1} ---> true
        {8,4,8,4,8} ---> true
        {9,842,613,2,613,842,9} ---> true
        {7,9,1,2} ---> false
        {0,0,0,7,0,0} ---> false

*/

bool isPalindrome(const List* list) {
    // TODO
    if (list->first == nullptr || list->first->next == nullptr)
    {
        return true;
    }
    std::stack <int> s;
    Node* tmp = list->first;
    while (tmp != nullptr)
    {
        s.push(tmp->data);
        tmp = tmp->next;
    }

    tmp = list->first;
    int i;
    while (tmp != nullptr)
    {
        i = s.top();
        s.pop(); 
        
        if (tmp->data != i)
        {
            return false;
        }
        tmp = tmp->next;
    }
    return true; // tento riadok zmente podla zadania, je tu len kvoli kompilacii
}


//-------------------------------------------------------------------------------------------------
// 7. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia vrati sucet hodnot prvych 'n' uzlov v zozname 'list'.

    VSTUPNE PODMIENKY:
        1) Plati, ze 'list' vzdy existuje (t.j. 'list' je rozny od 'nullptr').
        2) Zoznam 'list' moze byt lubovolne dlhy (0 a viac uzlov).
        3) Parameter 'n' nebude zaporny a nebude mat nikdy vyssiu hodnotu ako je realny pocet
           uzlov v zozname.

    PARAMETRE:
        [in] list - vstupny zretazeny zoznam
        [in] n - pocet uzlov od zaciatku zoznamu 'list', ktorych sucet sa vrati

    NAVRATOVA HODNOTA:
        Sucet hodnot prvych 'n' uzlov zoznamu 'list'. Ak 'n'=0, vtedy funkcia vrati hodnotu 0.

    PRIKLADY:
        {},0 ---> 0
        {1,2,3},0 ---> 0
        {6},1 ---> 6
        {5,6,7},2 ---> 11
        {4,4,4,4,4,4},6 ---> 24
        {-8,41,-314,21},3 ---> -281

*/

int sumNodes(const List* list, const int n) {
    // TODO
    int pocet = 0;
    Node* tmp = list->first;
    for (int i = 0; i < n; i++)
    {
        pocet += tmp->data;
        tmp = tmp->next;
    }

    return pocet; // tento riadok zmente podla zadania, je tu len kvoli kompilacii
}



//-------------------------------------------------------------------------------------------------
// 8. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia overi, ci zoznam 'list1' obsahuje vsetky hodnoty uzlov zoznamu 'list2'.
    Na poradi nezalezi.

    VSTUPNE PODMIENKY:
        1) Plati, ze 'list1' a 'list2' vzdy existuju (t.j. su rozne od 'nullptr').
        2) Zoznamy 'list1' a 'list2' mozu byt lubovolne dlhe (0 a viac uzlov).
        3) Zoznam 'list2' nebude obsahovat duplicitne uzly.

    PARAMETRE:
        [in] list1 - prvy zoznam
        [in] list2 - druhy zoznam

    NAVRATOVA HODNOTA:
        Pravdivostna hodnota 'true'/'false' podla toho, ci zoznam 'list1' obsahuje/neobsahuje
        vsetky hodnoty uzlov zoznamu 'list2'.

    PRIKLADY:
        {},{} ---> true
        {},{1,2,3} ---> false
        {1,2,3},{} ---> true
        {5,6,7},{5,6,8} ---> false
        {8,4,3},{3,8,4} ---> true
        {7,6,1,9,8},{1,6,8} ---> true
        {3,3,3,3,3},{2,3} ---> false
        {8,6,1,2,5,0,7,7},{7} ---> true
        {9,5,10},{41,10,5} ---> false

*/

bool contains(const List* list1, const List* list2) {
    // TODO
    if (list1->first == nullptr && list2->first != nullptr)
    {
        return false;
    }
    else if (list2->first == nullptr)
    {
        return true;
    }
    else if (list1->first==nullptr)
    {
        return false;
    }
    else
    {
        bool flag = 1;

        Node* tmp1 = list1->first;
        Node* tmp2 = list2->first;

        int i_tmp1 = 0;
        int i_tmp2 = 0;
        
        while (tmp1->next != nullptr || tmp2->next != nullptr)
        {
            if (tmp1->next != nullptr)
            {
                ++i_tmp1;
                tmp1 = tmp1->next;
            }
            if (tmp2->next != nullptr)
            {
                ++i_tmp2;
                tmp2 = tmp2->next;
            }
        }

        if (i_tmp1 <= i_tmp2)
        {
            tmp1 = list1->first;
            tmp2 = list2->first;
        }
        else
        {
            tmp1 = list2->first;
            tmp2 = list1->first;
        }

        do
        {
            do
            {
                if (tmp1->data == tmp2->data)
                {
                    flag = 0;
                    break;
                }
                tmp2 = tmp2->next;

            } while (tmp2!=nullptr);

            if (flag)
            {
                return false;
            }

            tmp1 = tmp1->next;
            tmp2 = list2->first;
            flag = 1;
        } while (tmp1 != nullptr);
    }

    return true; // tento riadok zmente podla zadania, je tu len kvoli kompilacii
}



//-------------------------------------------------------------------------------------------------
// 9. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia vytvori a vrati HLBOKU kopiu zoznamu 'list'. Vo vytvorenej kopii musia byt zachovane
    hodnoty a poradie uzlov z povodneho zoznamu.

    VSTUPNE PODMIENKY:
        1) Plati, ze 'list' vzdy existuje (t.j. je rozny od 'nullptr').
        2) Zoznam 'list' moze byt lubovolne dlhy (0 a viac uzlov).

    PARAMETRE:
        [in] list - adresa zoznamu, ktoreho hlboku kopiu treba vytvorit

    NAVRATOVA HODNOTA:
        Adresa zoznamu, ktory vznikol ako hlboka kopia vstupneho zoznamu 'list'. Kopiou
        prazdneho zoznamu je prazdny zoznam.

    PRIKLADY:
        Vstup = {}
        Vystup = {}

        Vstup = {1,2,3}
        Vystup = {1,2,3}
        Poznamka: adresy uzlov na rovnakych poziciach v oboch zoznamoch sa musia lisit
        (pretoze je to hlboka kopia)
*/

List* deepCopyList(const List* list) {
    // TODO
    List* n_list = new List();
    n_list->first = nullptr;
    Node* tmp = list->first;
    do
    {
        appendNode(n_list, tmp->data);
        tmp = tmp->next;
    } while (tmp != nullptr);

    return n_list; // tento riadok zmente podla zadania, je tu len kvoli kompilacii
}



//-------------------------------------------------------------------------------------------------
// 10. ULOHA (0.4 bodu)
//-------------------------------------------------------------------------------------------------
/*
    Funkcia najde posledny vyskyt uzla s hodnotou 'val' v zozname 'list' a vrati jeho adresu.
    Ak sa v zozname 'list' takyto uzol nenachadza, funkcia vrati 'nullptr'.

    VSTUPNE PODMIENKY:
        1) Plati, ze 'list' vzdy existuje (t.j. je rozny od 'nullptr').
        2) Zoznam 'list' moze byt lubovolne dlhy (0 a viac uzlov).

    PARAMETRE:
        [in] list - vstupny zretazeny zoznam
        [in] val - hladana hodnota uzla

    NAVRATOVA HODNOTA:
        Adresa posledneho vyskytu uzla s hodnotou 'val' v zozname 'list'.

    PRIKLADY (treba otestovat pomocou debuggera):
        {},2 ---> nullptr
        {5,4,3,2},1 ---> nullptr
        {-5,4,-9,66},4 ---> adresa 2. uzla
        {1,1,1,5,6,41},1 ---> adresa 3. uzla
        {8,6,7,3,2,6},6 ---> adresa 6. uzla
*/

Node* findLastNodeOccurrence(const List* list, const int val) {
    // TODO
    if (list->first != nullptr)
    {
        bool flag = 0;
        Node* tmp = list->first;
        Node* address = tmp;
        while (tmp->next != nullptr)
        {
            if (tmp->data == val)
            {
                address = tmp;
                flag = 1;
            }
            tmp = tmp->next;
        }
        if (flag)
        {
            delete tmp;
            return address;
        }
    }
    return nullptr; // tento riadok zmente podla zadania, je tu len kvoli kompilacii
}

//-------------------------------------------------------------------------------------------------
// TESTOVANIE
//-------------------------------------------------------------------------------------------------

// tu mozete doplnit pomocne testovacie funkcie a struktury

int main() {

    // tu mozete doplnit testovaci kod
    int pole[] = { 7,6,1,9,8 };
    int pole2[] = { 8,4,8,4,8 };
    //int* ptr = pole;
    ListData L_data;
    L_data.data = pole;
    L_data.len = sizeof(pole)/sizeof(int);
    ListData L_data2;
    L_data2.data = pole2;
    L_data2.len = sizeof(pole2) / sizeof(int);
    List* list = createList(&L_data);
    List* list2 = createList(&L_data2);
    //List* jliist = joinLists(list, list2); //ide
    //removeLastNode(list2); //ide
    //int sum = sumNodes(list2,2); //ide
    //bool b = contains(list,list2);//ide
    //Node* n = findLastNodeOccurrence(list,4); //ide
    //List* list3 = deepCopyList(list);
    insertNode(list,0);//ide
    std::cout << isPalindrome(list2) << endl;
    delete list;
    delete list2;
    //delete list3;

    return 0;
}