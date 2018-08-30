#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index_container.hpp>
#include <iostream>

struct Person {
    std::string firstName;
    std::string lastName;
};

std::ostream &operator<<(std::ostream &os, const Person &p) {
    os << "[" << p.firstName << ", " << p.lastName << "]";
    return os;
}

namespace bmi = boost::multi_index;

// Define tags for better readability
struct Sequenced;
struct SortByLastName;
struct SortByLastFirstName;

// clang-format off
using SortedPersons = bmi::multi_index_container<
    Person,
    bmi::indexed_by<
        // Sequence
        bmi::sequenced<
            bmi::tag<Sequenced>
        >,
        // Ordered descending by last name
        bmi::ordered_unique<
            bmi::tag<SortByLastName>,
            bmi::member<Person, std::string, &Person::lastName>,
            std::greater<std::string>
        >,
        // Ordered descending by last name and ascending by first name
        bmi::ordered_unique<
            bmi::tag<SortByLastFirstName>,
            bmi::composite_key<
                Person,
                bmi::member<Person, std::string, &Person::lastName>,
                bmi::member<Person, std::string, &Person::firstName>
            >,
            bmi::composite_key_compare<
                std::greater<std::string>,
                std::less<std::string>
            >
        >
    >
>;
// clang-format on

int main() {
    // Create container
    SortedPersons persons;

    // Fill container
    auto &sequenced = persons.get<Sequenced>();
    sequenced.push_back({"Jörg", "Hansen"});
    sequenced.push_back({"Moritz", "Jasper"});

    // insert() on ordered index corresponds to push_back() on sequenced index
    persons.get<SortByLastName>().insert({"Nikolai", "Wuttke"});

    // Print persons in insertion order
    std::cout << "Insertion order" << '\n';
    for (const auto &person : sequenced) {
        std::cout << person << '\n';
    }

    // Print persons order by last and first name
    std::cout << "Alphabetical order" << '\n';
    for (const auto &person : persons.get<SortByLastFirstName>()) {
        std::cout << person << '\n';
    }

    // Change "Jörg" to "Jochen" by modify()
    auto joerg = sequenced.begin();
    persons.modify(joerg, [&](Person &p) { p.firstName = "Jochen"; });

    // If a modification fails due to index clashes, the person is removed:
    //     persons.modify(joerg, [&](Person &p) { p.lastName = "Wuttke"; });

    // Find the person that was inserted right before "Wuttke"
    auto wuttke = persons.get<SortByLastFirstName>().find("Wuttke");
    // Alternative:
    // auto wuttke = persons.get<SortByLastFirstName>()
    //                      .find(std::make_tuple("Wuttke", "Nikolai"));
    auto before = persons.project<Sequenced>(wuttke);
    before--;
    std::cout << "Before Wuttke: " << *before << '\n';

    return 0;
}
