/**
 * @page SmartPropertiesTutorial_cs Smart Properties Tutoriál(CS)
 * @brief Jednoduchá cesta pro naučení Smart properties.
 *

<h3>Začínáme</h3>

Jeden malý kousek kódu asi řekne více než tísíc slov. Řekněme, že máme tři
proměnné a chceme mít v proměnné sum jejich součet a vždy aktuální.

@code{.cpp}

 AMPropertyPool pool;

 AMProperty<float> propk(pool, 1.11f);
 AMProperty<float> propl(pool, 2.22f);
 AMProperty<float> propm(pool, 3.33f);

 AMProperty<float> sum(pool, [&]{return propk+propl+propm;});

 cout << "k+l+m=" << (float)sum << endl;  //print sum, 6.66

 propl = 4.44f; //we add by 2.22

 cout << "k+l+m=" << sum() << endl;  //print new sum, 8.88

 @endcode

Všimněte si zachytávání parametrů pro lambda funkci referencí. Této lambda funkci říkejte funktor. Naopak, pokud budete
chtít předávat Smart property přes nějaký pointer, překladač uloží do funktoru ten
pointer. V takovém případě ho předejte hodnotou([=]).

Jak vidíte, stačí změnit jen proměnnou propl, sum se při příštím pokusu o čtení vypočete znovu.

Každá Smart property musí být svázána s nějakým poolem (AMSmartProperties::AMPropertyPool). Pool svazuje Smart properties dohromady
a zajišťuje jejich společnou funkčnost. Typicky, jedno okno má jeden pool.

Ještě zbývá vysletlit, co je to volání funkce za property (operator()). Jeden ze základních principů
Smart properties je, že lambda funkce nebere žádný parametr a vrací typ T, v našem případě float. Čtení ze
Smart property je tedy ve většině případů spojeno s přetypováním z AMSmartProperties::AMProperty <T> na T. Ve Smart properties jsou možné
tři způsoby, všechny jsou pro funkčnost rovnocenné. První je implicitní přetypování, jaké vidíme v tom funktoru na
sedmém řádku. Ačkoli sčítáme Smart properties, nakonec vrátíme float. Druhou možnost vidíme na řádku 9, kde explicitně
získáme uloženou hodnotu jako float. Přetypování na float tímto způsobem může být otravné, tak existuje ještě jedna možnost
-- operator(). Tak, jak je vidět na řádku 11.

Předchozí kód najdete @ref AMProperty_1.cpp.

<h3>Jak to úplně nedělat</h3>

@code{.cpp}

float k = 1.11f;
float l = 2.22f;
float m = 3.33f;

AMPropertyPool pool;
AMProperty<float> prop1(pool, [&]{return k+l+m;});

cout << "k+l+m=" << sum() << endl;//6.66

l = 4.44;

cout << "k+l+m=" << sum() << endl;//6.66!

 @endcode

 Když tohle skusíte, zjistíte, že se po změně proměnné l, nepřepočítal sum. Výsledky se totiž kešují.
 Znamená to, že se proměnné přepočítávají, jen pokud něco změníte, přesně pokud změníte nějaké Smart property
 v stejném poolu (AMSmartProperties::AMPropertyPool) funktor nebo hodnotu.

 Ačkoliv tento způsob není řešení úvodního problému, může se Vám k něčemu hodit -- k rozbití smyčky.

Předchozí kód najdete @ref AMProperty_2.cpp.


<h3>Smart properties a datové struktury</h3>

Vrať me se ale k prvnímu příkladu. Řekněme, že nechceme sčítat tři proměnné, ale procházet nějakou datovou strukturu, ve které
jsou uloženy Smart properties. Jde to, samozřejmě. Zde je příklad.

@code{.cpp}

AMPropertyPool pool;
std::list<AMProperty<int> > myList;

for(int i=0; i < 10; i++) {
	AMProperty<int> value(pool, i+1);
	myList.push_back(value);
}

AMProperty<int> sum(pool, [&]	{
	int sum = 0;
	for(std::list<AMProperty<int> >::iterator it = myList.begin(); it != myList.end(); it++) {
		sum += (int)*it;
	}
	return sum;
});

cout << "sum1 =" << sum() << endl; //55

std::list<AMProperty<int> >::iterator it = myList.begin();
std::advance(it, 4);
*it = 25; //previous value 5

cout << "sum2 =" << sum() << endl; //75

@endcode

 Předchozí kód najdete @ref AMProperty_3.cpp.


<h3>Zanořování Smart properties</h3>

Řekněme, že chceme mít třídu Point, která je sama Smart property a současně obsahuje,
Smart properties x a y.

@code{.cpp}
struct Point
{
    double x;
    double y;

};
@endcode

A kde jsou ty Smart Properties uvnitř? No, zvětšovaly by tu strukturu Point a omezovaly možnosti jejího použití.
Navíc, my bychom chtěli mít možnost dostat ze Smart property ten zákadní typ, tak jako, float ze AMProperty<float>.

Proto nadefinujeme ještě jednu strukturu.

@code{.cpp}
class PointSubproperties
{
public:
    AMProperty<double, double, Point, &Point::x> x;
    AMProperty<double, double, Point, &Point::y> y;

    AMPointSubproperties(AMPropertyPool& pool, Point* tPtr): x(pool, &tPtr->x), y(pool, &tPtr->y) {};
    ~AMPointSubproperties() {};

    void compute() {
        x.compute();
        y.compute();
    };
	void setPool(AMPropertyPool& pool) {
		x.setPool(pool);
		y.setPool(pool);
	}
    void setDescription(const char * desc);
    std::string to_string() const;
};

namespace std {

    template<> string to_string(::Point&);
    template<> string to_string(const ::Point&);
    template<> string to_string(::AMProperty<Point>&);;
}

@endcode

...a k ní ještě tento kód...

@code{.cpp}
#include "AMCoords.h"

namespace std {

	template<> string to_string(::Point& _x) {
	    return string("Point[")+to_string(_x.x)+string(",")+to_string(_x.y)+string("]");
	}

	template string to_string(::Point& _x);

	template<> string to_string(const ::Point& _x) {
	    return string("Point{")+to_string(_x.x)+string(",")+to_string(_x.y)+string("}");
	}

	template string to_string(const ::Point& _x);

	template<> string to_string(::AMProperty<Point>& _x) {
	    return _x.to_string();
	}

	template string to_string(::AMProperty<Point>& _x);

}

std::string PointSubproperties:: to_string() const
{
    std::ostringstream os;
            os<<"Props[x=";
            os<<x.to_string();
            os<<", y=";
            os<<y.to_string();
            os<<"]";
    return os.str();
};

void PointSubproperties::setDescription(const char * desc)
{
    x.setDescription(desc , ".x");
    y.setDescription(desc , ".y");
}
@endcode

-# Smart properties nadefinuje trošku modifikovaným způsobem. Řekneme jim, že jejich
nadřazená struktura je Point a jejich adresu v rámci této struktury (jako pointer na člena).

-# Nadřazená Smart property, AMProperty<Point>, bude volat náš konstruktor v tomto formátu,
kde  jako prní parametr je pool. která jen jedoduše zkopírujeme do subproperties a pointer na strukturu
Point. Ten použijeme k tomu, abychom zjstili skutečnou adresu subproperties a předali ji jen jako pointer.
Subproperty tedy nemá v sobě jakoukoli hodnotu a místo toho na ji jen ukazuje referencí.

-# Pak je nutno vytvořit několik triviálních metod, kde jen provedeme nějako činnost pro všechny
požadované subproperties. Jmenovitě compute(), setPool(pool), setDescription(description).

-# Pro debug, ale asi to bez nich nepůjde přeložit, je dobré implementovat metody to_string.
jednoduše vypíšu text a zahrnu do něj volání to_string i na subproperties. Provedu jako pro Point,
tak pro const Point, pro AMProperty<Point> a i pro PointSubproperties.

-# Nesmím zapomenout tyto čtyři metody instanciovat, když jsou cpp souboru.

Nakonec svůj výtvor použiji...

@code{.cpp}
AMProperty<Point, PointSubproperties> origin;
@endcode

<h3>A co když i celý Point je subproperty?</h3>
Oba přístupy můžeme kombinovat, třeba když chci vytvořit Smart property Rect.

@code{.cpp}
struct Rect
{
    AMPoint origin;
    ...

};

class RectSubproperties
{
	AMProperty<AMPoint, AMPointSubproperties, Rect, &Rect::origin> origin;
	...
}
@endcode
 */
