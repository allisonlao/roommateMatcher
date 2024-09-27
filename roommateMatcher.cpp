#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include <chrono>
#include <ctime>
#include <climits>
#include <fstream>

using StudentID = int;
static const StudentID kInvalidID = -1;
static const size_t kBedsPerRoom = 4;
static const size_t kMaxBuddyCount = 3;

using RoomID = int;

class Student
{
public:
    Student(const std::string &name, int grade, StudentID studentID) : mName(name),mGrade(grade),mStudentID(studentID)
    {
        mRoomNumber = -1;
    }

    bool requestBuddy(const std::string &buddyName)
    {
        assert(mBuddies.size() < kMaxBuddyCount);

        // Don't add duplicate buddy
        bool found = false;
        for(const std::string &existingBuddy : mBuddies)
        {
            if(buddyName.compare(existingBuddy) == 0)
            {
                found = true;
            }
        }
        if(found)
        {
            return false;
        }

        mBuddies.emplace_back(buddyName);
        return true;
    }

    bool isBuddyOf(const StudentID &other) const
    {
        // You can't be buddy of yourself
        assert(other != mStudentID);

        // Returns true if it is in the buddy list
        for(const StudentID &buddy : mBuddyIDs)
        {
            if(other == buddy)
            {
                return true;
            }
        }

        // Not in the buddy list
        return false;
    }

    // Returns how many students in others are my buddy
    size_t getBuddyCount(const std::vector<StudentID> &others) const
    {
        size_t buddyCount = 0;

        for(const StudentID &other : others)
        {
            // You can't be buddy of yourself
            if(other == mStudentID)
                continue;

            if(isBuddyOf(other))
            {
                buddyCount++;
            }
        }

        return buddyCount;
    }

    int score(const std::vector<Student> &allStudentInfos, const std::vector<StudentID> &others) const
    {
        int score = 0;

        for(const StudentID &other : others)
        {
            // You can't be buddy of yourself
            if(other == mStudentID)
                continue;

            // buddy adds one point
            if(isBuddyOf(other))
            {
                score += 5;
            }

            // two grades apart gets penalty points
            int gradeGap = mGrade - allStudentInfos[other].mGrade;
            if(gradeGap >= 2 || gradeGap <=-2)
            {
                score -= 500;
            }
        }

        return score;
    }

    const std::string &name() const { return mName;}
    int grade() const { return mGrade; }
    const StudentID &studentID() const { return mStudentID; }

    bool validateBuddyNames(std::vector<Student> &allStudentInfos);

    void assignRoom(RoomID roomID)
    {
        assert(roomID != kInvalidID);
        mRoomNumber = roomID;
    }

    void print() const
    {
        std::cout << mName << ":" << "\tGrade:" << mGrade << "\tAssigned Room:" << mRoomNumber;
        std::cout << "\tRequestedBuddy: {";
        for(size_t i = 0; i < mBuddies.size(); i++)
        {
            const std::string &buddy = mBuddies[i];
            std::cout << buddy;
            if(i != mBuddies.size()-1 )
            {
                std::cout << " , ";
            }
        }
        std::cout << "}\n";
    }

private:
    std::string mName;
    int mGrade;
    StudentID mStudentID;

    std::vector<std::string> mBuddies;
    std::vector<StudentID> mBuddyIDs;

    int mRoomNumber;
};
using StudentInfos = std::vector<Student>;


bool Student::validateBuddyNames(std::vector<Student> &allStudentInfos)
{
    for( const std::string &buddyName : mBuddies)
    {
        bool foundName = false;
        for(Student &student : allStudentInfos)
        {
            if(buddyName.compare(student.name()) == 0)
            {
                foundName = true;
                mBuddyIDs.emplace_back(student.studentID());
                break;
            }
        }
        if(!foundName)
        {
            std::cout<<"Student " << mName <<"'s buddy name " <<  buddyName << " is not on the student list";
            return false;
        }
    }
    return true;
}

class Room
{
public:
    Room(): mRoomID(-1){}
    Room(RoomID roomID): mRoomID(roomID){}

    bool assign(const std::vector<Student> &allStudentInfos, StudentID student)
    {
        assert(mStudents.size() < kBedsPerRoom);

        // Can't have two grade apart
        for(const StudentID existingStudent : mStudents)
        {
            assert(student != existingStudent);

#if false
            int gradeGap = allStudentInfos[student].grade() - allStudentInfos[existingStudent].grade();
            if(gradeGap >= 2 || gradeGap <= -2)
                return false;
#endif
        }

        mStudents.emplace_back(student);
        return true;
    }

    bool empty() const
    {
        return mStudents.size() == 0;
    }

    bool full() const
    {
        return mStudents.size() >= kBedsPerRoom;
    }

    int score(const std::vector<Student> &allStudentInfos) const
    {
        int total = 0;

        for(const StudentID &student : mStudents)
        {
            total += allStudentInfos[student].score(allStudentInfos, mStudents);
        }

        return total;
    }

    void print(const std::vector<Student> &allStudentInfos, std::ostringstream &names) const
    {
        assert(mStudents.size()<= kBedsPerRoom);
        names <<"{";
        for(size_t i=0; i< mStudents.size(); i++)
        {
            names << allStudentInfos[mStudents[i]].name();
            if(i != (mStudents.size()-1))
            {
                names << " , ";
            }
        }
        names <<"}";
    }

    void updateStudentInfo(std::vector<Student> &allStudentInfos) const
    {
        for(const StudentID &studentID : mStudents)
        {
            allStudentInfos[studentID].assignRoom(mRoomID);
        }
    }

    RoomID roomID() const { return mRoomID;}

private:
    std::vector<StudentID> mStudents;
    RoomID mRoomID;
};

class RoomAssignments
{
public:
    RoomAssignments():mScore(INT_MIN){};

    RoomAssignments(const std::vector<Student> &allStudentInfos, int roomCount) : mScore(0)
    {
        for(int roomID = 0; roomID < roomCount; roomID++)
        {
            mRooms.emplace_back(roomID);
        }
        generateRoomAssignment(allStudentInfos);
    }

    int score() const
    {
        return mScore;
    }

    RoomAssignments& operator=( const RoomAssignments& other )
    {
        mRooms = other.mRooms;
        mScore = other.mScore;
        return *this;
    }

    void print(const std::vector<Student> &allStudentInfos) const
    {
        std::cout <<"Total score for all rooms:" << mScore <<"\n";
        std::cout <<" List of Rooms:\n";
        for(const Room &room : mRooms)
        {
            std::ostringstream roomMates;
            room.print(allStudentInfos, roomMates);
            std::cout <<"\tRoom:" << room.roomID() << " Score:" << room.score(allStudentInfos) << " Studens:" << roomMates.str() << "\n";
        }
    }

    void updateStudentInfo(std::vector<Student> &allStudentInfos) const
    {
        for(const Room &room : mRooms)
        {
            room.updateStudentInfo(allStudentInfos);
        }
    }

private:
    void generateRoomAssignment(const std::vector<Student> &allStudentInfos)
    {
        //std::cout <<"\tgenerating one random room assignment ...";
        std::vector<bool> assigned(allStudentInfos.size(), false);
        int assignedStudentCount = 0;

        mScore = 0;
        for(int i=0; i<mRooms.size(); i++)
        {
            Room &room = mRooms[i];
            assert(room.empty());

            do {
                // Find a student that has not assigned.
                int studentID;
                do {
                    studentID = rand() % allStudentInfos.size();
                } while(assigned[studentID]);

                // assign this student to this room
                bool success = room.assign(allStudentInfos, studentID);
                if(success)
                {
                    assigned[studentID] = true;
                    assignedStudentCount++;
                }

                // Done if all students has been assigned a room
                if(assignedStudentCount >= allStudentInfos.size())
                    break;

            } while(!room.full());

            mScore += room.score(allStudentInfos);

            // Done if all students has been assigned a room
            if(assignedStudentCount >= allStudentInfos.size())
                break;
        }
    }

    std::vector<Room> mRooms;
    int mScore;
};

class DecaConference
{
public:
    DecaConference();
    void optimizeRoomAssignment();
    void printRoomAssignment() const;
    void printStudentInfos() const;

private:
    void readInputFile();
    void generateSimulationData();
    bool validateStudentInfo();

    RoomAssignments mCurrentRoomAssignment;

    int mStudentCount;
    int mRoomCount;

    std::vector<Student> mAllStudentInfos;

};

DecaConference::DecaConference():mStudentCount(0),mRoomCount(0)
{
#if true
    readInputFile();
#else
    generateSimulationData();
#endif
    validateStudentInfo();
}

void DecaConference::readInputFile() {
    std::fstream fin;
    fin.open("testing_data.csv", std::ios::in);

    std::vector<std::string> row;
    std::string line, word;
    fin >> mStudentCount;
    mRoomCount = (mStudentCount + kBedsPerRoom - 1) / kBedsPerRoom;
    getline(fin, line);
    for (int i = 0; i < mStudentCount; i++) {
        row.clear();
        getline(fin, line);
        std::stringstream s(line);
        while (getline(s, word, ',')) {
            row.push_back(word);
        }
        int studentID = std::stoi(row[1]);
        std::string name = row[2];
        int grade = std::stoi(row[6]);
        mAllStudentInfos.emplace_back(name, grade, studentID);

        int numBuddies = std::stoi(row[7]);
        for (int j = 0; j < numBuddies; j++) {
            std::string bname = row[8+3*j];
            mAllStudentInfos[studentID].requestBuddy(bname);
        }
    }
// row[index] and info associated with it
// {0: email, 1: studentID, 2: name, 6: grade, 7: numBuddies,
// (8, 11, 14): buddy names, (9, 12, 15): buddy grades, (10, 13, 16): buddy IDs}
    std::cout << "\tDONE\n";
}

bool DecaConference::validateStudentInfo()
{
    for(Student &student : mAllStudentInfos)
    {
        bool valid = student.validateBuddyNames(mAllStudentInfos);
        if(!valid)
        {
            std::cout << " student "<< student.name() <<" validation failed";
            return false;
        }
    }
    return true;
}

void DecaConference::optimizeRoomAssignment()
{
    std::cout <<"Optimizing room assignment ...\n";
    auto start = std::chrono::system_clock::now();
    auto lastPrintTime = std::chrono::system_clock::now();
    size_t tryCount = 0;
    while(true)
    {
        RoomAssignments randomRoomAssignment(mAllStudentInfos, mRoomCount);
        tryCount++;
        if(randomRoomAssignment.score() > mCurrentRoomAssignment.score())
        {
            std::cout << " found max score: " << randomRoomAssignment.score() << "\n";
            mCurrentRoomAssignment = randomRoomAssignment;
            start = std::chrono::system_clock::now();
            lastPrintTime = std::chrono::system_clock::now();
        }

        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;

        constexpr int kSecondsToExit = 25;
        if(elapsed_seconds.count() > kSecondsToExit)
        {
            std::cout<<"\nCan't find better assignment for " << kSecondsToExit << " seconds. Finishing.\n";
            break;
        }
        else
        {
            std::chrono::duration<double> print_elapsed_seconds = std::chrono::system_clock::now() - lastPrintTime;
            if(print_elapsed_seconds.count() > 1)
            {
                std::cout<<"Still searching... tryCount= " << tryCount << "\n";
                lastPrintTime = std::chrono::system_clock::now();
            }

        }
    }

    // Now update student information with room assignment
    mCurrentRoomAssignment.updateStudentInfo(mAllStudentInfos);
}

void DecaConference::printRoomAssignment() const
{
    mCurrentRoomAssignment.print(mAllStudentInfos);
}

void DecaConference::printStudentInfos() const
{
    std::cout <<"\nAll students: \n";
    for(const Student &student : mAllStudentInfos)
    {
        student.print();
    }
}

int main(int argc, const char * argv[])
{
    DecaConference decaConference;

    decaConference.optimizeRoomAssignment();
    decaConference.printRoomAssignment();
    decaConference.printStudentInfos();

    return 0;
}



