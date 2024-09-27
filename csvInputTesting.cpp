//
// Created by Grace Xu on 9/26/24.
//

//#include "csvInputTesting.h"
#include <bits/stdc++.h>
using namespace std;

//class csvInputTesting {
    int main() {
        std::fstream fin;
        fin.open("testing_data.csv", std::ios::in);

        std::vector<std::string> row;
        std::string line, word;
        int mStudentCount;
        fin >> mStudentCount;
        getline(fin, line);
        for (int i = 0; i < mStudentCount; i++) {
            row.clear();
            getline(fin, line);
//            cout << "line: " << line << "\n";
            std::stringstream s(line);
            while (getline(s, word, ',')) {
                row.push_back(word);
            }
            int studentID = std::stoi(row[1]);
            std::string name = row[2];
            int grade = std::stoi(row[6]);
            int numBuddies = std::stoi(row[7]);
            for (int j = 0; j < numBuddies; j++) {
                string bname = row[8+3*j];
                int idx = 10+2*j;
                int buddyID = stoi(row[10+3*j]);
                std::cout << "student: " << name << ", ";
                std::cout << "buddy name: " << bname << ", Buddy id: " << buddyID << "\n";
            }
        }
//        std::cout << row[1] << "\n"; // studentID
//        std::cout << row[2] << "\n"; // name
//        std::cout << row[6] << "\n"; // grade
//        std::cout << row[7] << "\n"; // numBuddies
//        std::cout << row[8] << "\n"; // buddy1 name
//        std::cout << row[9] << "\n"; // buddy1 grade
//        std::cout << row[10] << "\n"; // buddy1 id
//        std::cout << row[11] << "\n"; // buddy2 name
//        std::cout << row[12] << "\n"; // buddy2 grade
//        std::cout << row[13] << "\n"; // buddy2 id
//        std::cout << row[14] << "\n"; // buddy3 name
//        std::cout << row[15] << "\n"; // buddy3 grade
//        std::cout << row[16] << "\n"; // buddy3 id
        return 0;
    }



//    void test() {
//        fstream fin;
//        fin.open("testing_data.csv", std::ios::in);
//
//        vector<string> row;
//        string line, word, temp;
//        while (fin >> temp) {
//            row.clear();
//            getline(fin, line);
//            stringstream s(line);
//            while (getline(s, word, ',')) {
//                row.push_back(word);
//            }
//            cout << row[0] << "\n";
//            cout << row[1] << "\n";
//            cout << row[2] << "\n";
//            cout << row[3] << "\n";
//            cout << row[4] << "\n";
//            cout << row[5] << "\n";
//            cout << row[6] << "\n";
//
//
//        }
//    }

//};

//void DecaConference::readInputFile() {
//    std::fstream fout;
//    fout.open("testing_data.csv", std::ios::in);
//    int studentID, grade, numBuddies;
//    std::cin >> mStudentCount;
//    std::string name, email, filler;
//    mRoomCount = (mStudentCount + kBedsPerRoom - 1) / kBedsPerRoom;
//    for (int i = 0; i < mStudentCount; i++) {
//        std::cin >> email
//                 >> studentID
//                 >> name
//                 >> filler
//                 >> filler
//                 >> filler
//                 >> grade
//                 >> numBuddies;
//        std::cout << "student" << i;
//        mAllStudentInfos.emplace_back(name, grade, studentID);
//        std::string buddyName;
//        int buddyGrade;
//        std::vector<StudentID> buddyList;
//        for (int j = 0; i < numBuddies; j++) {
//            std::cin >> buddyName >> buddyGrade;
//            mAllStudentInfos[studentID].requestBuddy(buddyName);
//        }
//    }
//    std::cout << "Done\n";
//}

