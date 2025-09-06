#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>

struct AdmissionCriteria {
    int minScience = 0, minCommerce = 0, minArts = 0;
    int seatsScience = 0, seatsCommerce = 0, seatsArts = 0;

    void input();
    void display() const;
    void save() const;
    bool load();
};

void AdmissionCriteria::input() {
    std::cout << "Set minimum marks (percentage) required for each stream and number of seats:\n";
    std::cout << "Minimum for Science: "; std::cin >> minScience;
    std::cout << "Seats for Science: "; std::cin >> seatsScience;
    std::cout << "Minimum for Commerce: "; std::cin >> minCommerce;
    std::cout << "Seats for Commerce: "; std::cin >> seatsCommerce;
    std::cout << "Minimum for Arts: "; std::cin >> minArts;
    std::cout << "Seats for Arts: "; std::cin >> seatsArts;
}

void AdmissionCriteria::display() const {
    std::cout << "== Admission Criteria ==\n";
    std::cout << "Science: Min " << minScience << "%, Seats: " << seatsScience << '\n';
    std::cout << "Commerce: Min " << minCommerce << "%, Seats: " << seatsCommerce << '\n';
    std::cout << "Arts: Min " << minArts << "%, Seats: " << seatsArts << '\n';
}

void AdmissionCriteria::save() const {
    std::ofstream fout("criteria.dat", std::ios::binary);
    fout.write(reinterpret_cast<const char*>(this), sizeof(*this));
}

bool AdmissionCriteria::load() {
    std::ifstream fin("criteria.dat", std::ios::binary);
    if (fin) {
        fin.read(reinterpret_cast<char*>(this), sizeof(*this));
        return true;
    }
    return false;
}

class Student {
public:
    std::string name;
    int regNo = 0;
    int marksEng = 0, marksMath = 0, marksSci = 0, marksSSt = 0, marksLang = 0;
    int streamPref = -1;
    int allottedStream = 0; // 0-none, 1-Science, 2-Commerce, 3-Arts

    void input();
    void display() const;
    int avg() const;
    void allotStream(const AdmissionCriteria&);
    static std::string streamName(int code);
};

void Student::input() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << "Name: "; std::getline(std::cin, name);
    std::cout << "Registration No.: "; std::cin >> regNo;
    std::cout << "Marks in English: "; std::cin >> marksEng;
    std::cout << "Marks in Mathematics: "; std::cin >> marksMath;
    std::cout << "Marks in Science: "; std::cin >> marksSci;
    std::cout << "Marks in Social Studies: "; std::cin >> marksSSt;
    std::cout << "Marks in Language: "; std::cin >> marksLang;
    std::cout << "Preferred Stream (1-Science, 2-Commerce, 3-Arts): "; std::cin >> streamPref;
    allottedStream = 0;
}

void Student::display() const {
    std::cout << "---------------------------------\n";
    std::cout << "Name: " << name << "\nRegistration No.: " << regNo << '\n';
    std::cout << "Average Marks: " << avg() << "%\n";
    std::cout << "Preferred Stream: " << streamName(streamPref) << '\n';
    std::cout << "Allotted Stream: " << streamName(allottedStream) << '\n';
    std::cout << "---------------------------------\n";
}

int Student::avg() const {
    return (marksEng + marksMath + marksSci + marksSSt + marksLang) / 5;
}

void Student::allotStream(const AdmissionCriteria& crit) {
    int percentage = avg();
    switch (streamPref) {
        case 1: allottedStream = (percentage >= crit.minScience) ? 1 : 0; break;
        case 2: allottedStream = (percentage >= crit.minCommerce) ? 2 : 0; break;
        case 3: allottedStream = (percentage >= crit.minArts) ? 3 : 0; break;
        default: allottedStream = 0; break;
    }
}

std::string Student::streamName(int code) {
    switch (code) {
        case 1: return "Science";
        case 2: return "Commerce";
        case 3: return "Arts";
        default: return "None";
    }
}

void saveStudent(const Student& s) {
    std::ofstream fout("students.dat", std::ios::binary | std::ios::app);
    fout.write(reinterpret_cast<const char*>(&s), sizeof(s));
}

std::vector<Student> loadStudents() {
    std::vector<Student> students;
    std::ifstream fin("students.dat", std::ios::binary);
    Student s;
    while (fin.read(reinterpret_cast<char*>(&s), sizeof(s)))
        students.push_back(s);
    return students;
}

void saveMeritList(const std::vector<Student>& students, int streamCode, const std::string& filename) {
    std::ofstream fout(filename, std::ios::binary);
    for (const auto& s : students)
        if (s.allottedStream == streamCode)
            fout.write(reinterpret_cast<const char*>(&s), sizeof(s));
}

void displayStudentsStream(const std::string& filename) {
    std::ifstream fin(filename, std::ios::binary);
    Student s;
    while (fin.read(reinterpret_cast<char*>(&s), sizeof(s)))
        s.display();
}

void showMenu() {
    std::cout << "\n=== Student Admission/Stream Allocation System ===\n";
    std::cout << "1. Set Admission Criteria\n";
    std::cout << "2. Enter Student Data\n";
    std::cout << "3. Allot Streams\n";
    std::cout << "4. Show Admission Criteria\n";
    std::cout << "5. Display Students by Stream\n";
    std::cout << "6. Show Registered Students\n";
    std::cout << "7. Create/Show Merit List\n";
    std::cout << "8. Exit\n";
    std::cout << "Select Option: ";
}

int main() {
    AdmissionCriteria criteria;
    criteria.load();
    int choice;

    do {
        showMenu();
        std::cin >> choice;
        switch (choice) {
            case 1:
                criteria.input();
                criteria.save();
                break;
            case 2: {
                char more = 'y';
                while (more == 'y' || more == 'Y') {
                    Student s;
                    s.input();
                    saveStudent(s);
                    std::cout << "Add more students? (y/n): ";
                    std::cin >> more;
                }
                break;
            }
            case 3: {
                std::vector<Student> students = loadStudents();
                std::ofstream fout("students.dat", std::ios::binary | std::ios::trunc);
                for (auto& s : students) {
                    s.allotStream(criteria);
                    fout.write(reinterpret_cast<const char*>(&s), sizeof(s));
                }
                std::cout << "Streams allotted based on current criteria.\n";
                break;
            }
            case 4:
                if (criteria.load())
                    criteria.display();
                else
                    std::cout << "No admission criteria set yet.\n";
                break;
            case 5: {
                int sCode;
                std::cout << "Show which stream? (1-Science, 2-Commerce, 3-Arts): ";
                std::cin >> sCode;
                std::string fname = (sCode == 1) ? "sci_merit.dat" : (sCode == 2) ? "com_merit.dat" : "arts_merit.dat";
                displayStudentsStream(fname);
                break;
            }
            case 6: {
                auto students = loadStudents();
                for (const auto& s : students) s.display();
                break;
            }
            case 7: {
                std::vector<Student> students = loadStudents();
                int sCode;
                std::cout << "Generate merit list for (1-Science, 2-Commerce, 3-Arts): ";
                std::cin >> sCode;
                std::vector<Student> filtered;
                for (const auto& s : students)
                    if (s.allottedStream == sCode)
                        filtered.push_back(s);
                std::sort(filtered.begin(), filtered.end(), [](const Student& a, const Student& b) {
                    return a.avg() > b.avg();
                });
                std::string fname = (sCode == 1) ? "sci_merit.dat" : (sCode == 2) ? "com_merit.dat" : "arts_merit.dat";
                saveMeritList(filtered, sCode, fname);
                displayStudentsStream(fname);
                break;
            }
            case 8:
                std::cout << "Goodbye!\n"; break;
            default:
                std::cout << "Invalid choice.\n";
        }
    } while (choice != 8);

    return 0;
}