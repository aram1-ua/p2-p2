#include <iostream>
#include <fstream> // Para usar ficheros
#include <vector> // Para usar vector
#include <cstring>
#include <iomanip>

using namespace std;

const int KMAXNIF=10;
const int KMAXNAME=50;
const int KMAXTELEPHONE=14;

// Registro para los pacientes
struct Patient{
    string nif;
    string name;
    string telephone;
};

// Registro para los pacientes en fichero binario
struct PatientBin{
    char nif[KMAXNIF];
    char name[KMAXNAME];
    char telephone[KMAXTELEPHONE];
};

// Registro para las fechas
struct Date{
    int day;
    int month;
    int year;
};

// Registro para las analíticas
struct Analysis{
    unsigned int id;
    char nif[KMAXNIF];
    Date dateAnalysis ;
    float weight;
    float height;
};

// Registro para la base de datos
struct Database{
    unsigned int nextId = 1;
    vector<Patient> patients;
    vector<Analysis> analysis;
};

// Tipos de errores posibles
enum Error {
    ERR_ARGS,
    ERR_FILE,
    ERR_OPTION,
    ERR_PATIENT_EXISTS,
    ERR_PATIENT_NOT_EXISTS,
    ERR_WRONG_DATE,
    ERR_WRONG_NAME,
    ERR_WRONG_NIF,
    ERR_WRONG_NUMBER,
    ERR_WRONG_TELEPHONE
};


/*
Función que muestra los distintos tipos de errores posibles
e: tipo de error a mostrar
return: void
*/
void error(Error e){
    switch (e){
        case ERR_ARGS: cout << "ERROR: wrong arguments" << endl;
            break;
        case ERR_FILE: cout << "ERROR: cannot open file" << endl;
            break;
        case ERR_OPTION: cout << "ERROR: wrong option" << endl;
            break;
        case ERR_PATIENT_EXISTS: cout << "ERROR: patient already exists" << endl;
            break;
        case ERR_PATIENT_NOT_EXISTS: cout << "ERROR: patient does not exist" << endl;
            break;
        case ERR_WRONG_DATE: cout << "ERROR: wrong date" << endl;
            break;
        case ERR_WRONG_NAME: cout << "ERROR: wrong name " << endl;
            break;
        case ERR_WRONG_NIF: cout << "ERROR: wrong NIF " << endl;
            break;
        case ERR_WRONG_NUMBER: cout << "ERROR: wrong name " << endl;
            break;
        case ERR_WRONG_TELEPHONE: cout << "ERROR: wrong telephone number" << endl;
    }
}

int searchPatient(const Database &db, const string &nif){
    for (size_t i = 0; i < db.patients.size(); i++) {
        if (db.patients[i].nif == nif) {
            return i;
        }
    }
    return -1;
}


bool isValidNif(const string &nif){
    if(nif.length() != 9){
        return false;
    }
    for(int i= 0; i < 8; i++){
        if(!isdigit(nif[i])) {
            return false;
        }
    }
    if (!isalpha(nif[8])){
        return false;
    }
    return true;

}

void addPatient(Database &db){
    Patient patient;
    bool validNif = false;
    bool validName = false;
    bool validTele = false;

    do{
        cout << "Enter NIF: ";
        cin.ignore();
        getline(cin, patient.nif);

        if(patient.nif.empty()){
            validNif = true;
            return;
        }
        
        if(!isValidNif(patient.nif) && !patient.nif.empty()){
            error(ERR_WRONG_NIF);
            validNif = false;
        }

        if(searchPatient(db, patient.nif) != -1){
            error(ERR_PATIENT_EXISTS);
            validNif = false;
        }
    }while(validNif == false);

    cin.ignore();
    do{
        cout << "Enter name: ";
        getline(cin, patient.name);

        if (patient.name.length() < 3){
            validName = false;
            error(ERR_WRONG_NAME);
        }else{
            validName = true;
        }
    }while(validName == false);

    do{
        cout << "Enter telephone: ";
        cin >> patient.telephone;
        if (patient.telephone[0] != '+' || patient.telephone.length() < 11 || patient.telephone.length() > 13) {
            validTele = false;
            error(ERR_WRONG_TELEPHONE);
        }else{
            validTele = true;
        }
    }while(validTele == false);

    db.patients.push_back(patient);

}


void viewPatient(const Database &db) {
    string nif;
    int index;
    bool validNif = false;
    
    do{
        cout << "Enter NIF: ";
        cin.ignore();
        getline(cin, nif);

        if(nif.empty()){
            validNif = true;
            return;
        }
        index = searchPatient(db, nif);
        if (index == -1 && !nif.empty()) {
            error(ERR_PATIENT_NOT_EXISTS);
            validNif = false;
        }
    }while(validNif == false);

    if(index != -1){
        const Patient &p = db.patients[index];
        cout << "NIF: " << p.nif << endl;
        cout << "Name: " << p.name << endl;
        cout << "Telephone: " << p.telephone << endl;
        cout << "Id\tDate\tHeight\tWeight" << endl;
    /*for(size_t i = 0; i ){
        //FALTAAA
    }*/
    }
}

void deletePatient(Database &db) {
    string nif;
    int index;
    bool validNif = false;
    
    do{
        cout << "Enter NIF: ";
        cin.ignore();
        getline(cin, nif);

        if(nif.empty()){
            validNif = true;
            return;
        }

        index = searchPatient(db, nif);

        if (index == -1 && !nif.empty()) {
            error(ERR_PATIENT_NOT_EXISTS);
            validNif = false;
        }else{
            validNif = true;
        }
    }while(validNif == false);

    if(index != -1){
        db.patients.erase(db.patients.begin() + index);

        // Remove all associated analyses (Simple loop method)
        for (size_t i = 0; i < db.analysis.size(); ) {
            if (db.analysis[i].nif == nif) {
                db.analysis.erase(db.analysis.begin() + i);  // Remove analysis
            } else {
                i++;  // Only move to the next element if no deletion occurred
            }
        }
    }
    

}

void savePatients(const Database &db) {
    ofstream file("patients.bin", ios::binary);  
    if (!file) {
        error(ERR_FILE);  
        return;
    }

    for (const auto &p : db.patients) {
        PatientBin pbin = {};  

        strncpy(pbin.nif, p.nif.c_str(), KMAXNIF - 1);
        strncpy(pbin.name, p.name.c_str(), KMAXNAME - 1);
        strncpy(pbin.telephone, p.telephone.c_str(), KMAXTELEPHONE - 1);

        // Ensure null termination
        pbin.nif[KMAXNIF - 1] = '\0';
        pbin.name[KMAXNAME - 1] = '\0';
        pbin.telephone[KMAXTELEPHONE - 1] = '\0';

        file.write(reinterpret_cast<const char*>(&pbin), sizeof(PatientBin));  
    }

    file.close();
}

void addAnalysis(Database &db){
    Analysis a;
    string nif;
    int index;
    bool validNif = false;
    bool validDate = false;
    bool validWeight = false;
    bool validHeight = false;
    
    do{
        cout << "Enter NIF: ";
        cin.ignore();
        getline(cin, nif);

        if(nif.empty()){
            validNif = true;
            return;
        }

        index = searchPatient(db, nif);

        if (index == -1 && !nif.empty()) {
            error(ERR_PATIENT_NOT_EXISTS);
            validNif = false;
        }else{
            validNif = true;
        }
    }while(validNif == false);

    do{
        cout << "Enter date (day/month/year): ";
        cin >> a.dateAnalysis.day >> a.dateAnalysis.month >> a.dateAnalysis.year;
        if (cin.fail()) {  // If input fails, clear it
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            error(ERR_WRONG_DATE);
            continue;
        }

        if (a.dateAnalysis.day < 1 || a.dateAnalysis.day > 31 || 
            a.dateAnalysis.month < 1 || a.dateAnalysis.month > 12 ||
            a.dateAnalysis.year < 2025 || a.dateAnalysis.year > 2050) {
            error(ERR_WRONG_DATE);
            validDate = false;
        } else {
            validDate = true;
        }
    }while(validDate == false);
    
    do{
        cout << "Enter weight: ";
        cin >> a.weight;
        if(a.weight < 0){
            error(ERR_WRONG_NUMBER);
            validWeight = false;
        }
    }while(validWeight == false);

    do{
        cout << "Enter height: ";
        cin >> a.height;
        if(a.height < 0){
            error(ERR_WRONG_NUMBER);
            validHeight = false;
        }
    }while(validHeight == false);
    a.id = db.nextId++;
    db.analysis.push_back(a);

}

void exportAnalysis(const Database &db) {
    ofstream file("analysis.bin", ios::binary);
    if (!file) {
        error(ERR_FILE);
        return;
    }
    for (const auto &a : db.analysis) {
        file.write(reinterpret_cast<const char*>(&a), sizeof(Analysis));
    }
    file.close();
    cout << "Analysis exported successfully!\n";
}


void importAnalysis(Database &db) {
    ifstream file("analysis.bin", ios::binary);
    if (!file) {
        error(ERR_FILE);
        return;
    }
    Analysis a;
    while (file.read(reinterpret_cast<char*>(&a), sizeof(Analysis))) {
        if (searchPatient(db, a.nif) == -1) {
            ofstream wrongFile("wrong_patients.txt", ios::app);
            if (!wrongFile) {
                error(ERR_FILE);
                return;
            }
            wrongFile << a.nif << endl;
            wrongFile.close();
        } else {
            a.id = db.nextId++;
            db.analysis.push_back(a);
        }
    }
    file.close();
    cout << "Analysis imported successfully!\n";
}


void statistics(const Database &db) {
    if (db.analysis.empty()) {  // Check if there are any analyses
        cout << "No analysis records available.\n";
        return;
    }

    ofstream file("statistics.txt");
    if (!file) {  // Check if file opened successfully
        error(ERR_FILE);
        return;
    }

    for (const auto &a : db.analysis) {
        float heightM = a.height / 100.0;  // Convert cm to meters
        float bmi = a.weight / (heightM * heightM);
        
        string category;
        if (bmi < 18.5) category = "Underweight";
        else if (bmi < 25) category = "Healthy";
        else if (bmi < 30) category = "Overweight";
        else category = "Obesity";

        // Format and display output
        cout << a.nif << ";" << setw(2) << setfill('0') << a.dateAnalysis.day << "/"
             << setw(2) << setfill('0') << a.dateAnalysis.month << "/" << a.dateAnalysis.year
             << ";" << a.weight << ";" << a.height << ";" 
             << fixed << setprecision(1) << bmi << ";" << category << "\n";

        // Write the same formatted output to file
        file << a.nif << ";" << setw(2) << setfill('0') << a.dateAnalysis.day << "/"
             << setw(2) << setfill('0') << a.dateAnalysis.month << "/" << a.dateAnalysis.year
             << ";" << a.weight << ";" << a.height << ";" 
             << fixed << setprecision(1) << bmi << ";" << category << "\n";
    }

    file.close();
    cout << "Statistics saved to statistics.txt!\n";
}



void importFromTextFile(Database &db, const string &filename) {
    ifstream file(filename);
    if (!file) {
        error(ERR_FILE);
        return;
    }

    string line, nif;
    Analysis a;
    while (getline(file, line)) {
        stringstream ss(line);
        getline(ss, nif, ';');
        ss >> a.dateAnalysis.day;
        ss.ignore();
        ss >> a.dateAnalysis.month;
        ss.ignore();
        ss >> a.dateAnalysis.year;
        ss.ignore();
        ss >> a.weight;
        ss.ignore();
        ss >> a.height;

        if (searchPatient(db, nif) == -1) {
            ofstream wrongFile("wrong_patients.txt", ios::app);
            if (!wrongFile) {
                error(ERR_FILE);
                return;
            }
            wrongFile << nif << endl;
            wrongFile.close();
        } else {
            strncpy(a.nif, nif.c_str(), KMAXNIF - 1);
            a.nif[KMAXNIF - 1] = '\0';
            a.id = db.nextId++;
            db.analysis.push_back(a);
        }
    }
    file.close();
}




/*
Función que muestra el menú de opciones
return: void
*/
void showMenu() {
    cout << "1- Add patient" << endl
         << "2- View Patient" << endl
         << "3- Delete patient" << endl
         << "4- Save patients" << endl
         << "5- Add analysis" << endl
         << "6- Export analysis" << endl
         << "7- Import analysis" << endl
         << "8- Statistics" << endl
         << "q- Quit" << endl
         << "Option: ";
}

/*
Función principal: Tendrás que añadir más código tuyo
return: 0
*/
int main(int argc, char *argv[]){
    Database db;
    db.nextId = 1;
    bool importFlag = false, statsFlag = false;
    string filename;
    char option;

    // Process command-line arguments
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-f") {  // Import file
            if (i + 1 < argc) {
                filename = argv[++i];
                importFlag = true;
            } else {
                error(ERR_ARGS);
                return 1;
            }
        } else if (arg == "-s") {  // Show statistics
            statsFlag = true;
        } else {
            error(ERR_ARGS);  // Invalid argument
            return 1;
        }
    }

    // Validate arguments
    if (statsFlag && !importFlag) {
        error(ERR_ARGS);  // -s must be used with -f
        return 1;
    }

    // Load patients.bin at the start
    importAnalysis(db);

    // If -f was provided, import data from the specified file
    if (importFlag) {
        importFromTextFile(db, filename);
    }

    // If -s was provided, display statistics and exit
    if (statsFlag) {
        statistics(db);
        return 0;
    }

    do{
        showMenu();
        cin >> option;
        cin.ignore();
        
        switch(option){
            case '1': addPatient(db);// Llamar a la función "addPatient" para añadir una nueva ficha de paciente
                break;
            case '2': viewPatient(db);// Llamar a la función "viewPatient" para ver la información de un paciente
                break;
            case '3': deletePatient(db);// Llamar a la función "deletePatient" para eliminar una ficha de paciente
                break;
            case '4': savePatients(db);// Llamar a la función "savePatients" para guardar las fichas de pacientes en fichero binario
                break;
            case '5': addAnalysis(db);// Llamar a la función "addAnalysis" para anadir una analítica
                break;
            case '6': exportAnalysis(db);// Llamar a la función "exportAnalysis" para exportar las analiticas realizadas a fichero binario                
                break;
            case '7': importAnalysis(db);// Llamar a la función "importAnalysis" para importar las analiticas en fichero binario
                break;
            case '8': statistics(db);// Llamar a la función "statistics" para guardar las preguntas en fichero
                break;
            case 'q': // Salir del programa 
                break;
            default: error(ERR_OPTION);
        }
    }while(option!='q');
   
    return 0;
}