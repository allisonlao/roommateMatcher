import random

import pandas as pd
from typing import List, Dict

# Constants
kInvalidID = -1
kBedsPerRoom = 4
kMaxBuddyCount = 3

# Student Class
class Student:
    def __init__(self, name: str, grade: int, studentID: int):
        self.name = name
        self.grade = grade
        self.studentID = studentID
        self.roomNumber = kInvalidID
        self.buddies = []
        self.buddyIDs = []

    def request_buddy(self, buddy_name: str, buddy_grade: int) -> bool:
        if len(self.buddies) >= kMaxBuddyCount:
            return False
        self.buddies.append((buddy_name, buddy_grade))
        return True

    def is_buddy_of(self, other_id: int) -> bool:
        return other_id in self.buddyIDs

    def get_buddy_count(self, other_ids: List[int]) -> int:
        return sum(1 for other in other_ids if self.is_buddy_of(other))

    def score(self, all_students: List['Student'], others: List[int]) -> int:
        score = 0
        num_not_buddy = 0

        for other_id in others:
            if other_id == self.studentID:
                continue

            if not self.is_buddy_of(other_id):
                num_not_buddy += 1

            buddy_count = all_students[other_id].get_buddy_count(others)
            if buddy_count > 0:
                score -= num_not_buddy / buddy_count

            grade_gap = abs(self.grade - all_students[other_id].grade)
            if grade_gap >= 2:
                score -= 50

        return score

    def assign_room(self, roomID: int):
        self.roomNumber = roomID

    def validate_buddy_names(self, all_students: List['Student']) -> bool:
        for buddy_name, buddy_grade in self.buddies:
            found = False
            for student in all_students:
                if buddy_name == student.name and buddy_grade == student.grade:
                    self.buddyIDs.append(student.studentID)
                    found = True
                    break
            if not found:
                print(f"Student {self.name}'s buddy {buddy_name} (Grade {buddy_grade}) is not in the student list")
                return False
        return True

    def __repr__(self):
        return f"Student({self.name}, Grade: {self.grade}, Room: {self.roomNumber}, Buddies: {self.buddies})"


# Room Class
class Room:
    def __init__(self, roomID: int):
        self.roomID = roomID
        self.students = []

    def assign(self, all_students: List[Student], studentID: int) -> bool:
        if len(self.students) >= kBedsPerRoom:
            return False
        for existing_student in self.students:
            grade_gap = abs(all_students[studentID].grade - all_students[existing_student].grade)
            if grade_gap >= 2:
                return False
        self.students.append(studentID)
        return True

    def score(self, all_students: List[Student]) -> int:
        return sum(all_students[studentID].score(all_students, self.students) for studentID in self.students)

    def __repr__(self):
        return f"Room({self.roomID}, Students: {self.students})"


# RoomAssignments Class
class RoomAssignments:
    def __init__(self, all_students: List[Student], room_count: int):
        self.rooms = [Room(roomID) for roomID in range(room_count)]
        self.score = 0
        self.generate_room_assignment(all_students)

    def generate_room_assignment(self, all_students: List[Student]):
        assigned = [False] * len(all_students)
        assigned_student_count = 0

        students_by_grade: Dict[int, List[int]] = {}
        for student in all_students:
            students_by_grade.setdefault(student.grade, []).append(student.studentID)

        for room in self.rooms:
            if assigned_student_count >= len(all_students):
                break

            while not room.assign(all_students, random.choice(students_by_grade[student.grade])):
                random_student = random.choice(students_by_grade[student.grade])
                if room.assign(all_students, random_student):
                    assigned[random_student] = True
                    assigned_student_count += 1

            # After assigning students to the room, calculate the score for the room
            self.score += room.score(all_students)

    def print(self, all_students: List[Student]):
        print(f"Total score for all rooms: {self.score}")
        print("List of Rooms:")
        for room in self.rooms:
            room_students = ', '.join(f"{all_students[studentID].name} (Grade {all_students[studentID].grade})"
                                      for studentID in room.students)
            room_score = room.score(all_students)
            print(f"Room {room.roomID}: Score = {room_score}, Students = {room_students}")

    def update_student_info(self, all_students: List[Student]):
        for room in self.rooms:
            for studentID in room.students:
                all_students[studentID].assign_room(room.roomID)

# Helper Functions
def print_students(students: List[Student]):
    print("Printing Students:")
    for student in students:
        print(student)

# Read students and buddies from an Excel file
def load_students_from_excel(file_name: str) -> List[Student]:
    df = pd.read_excel(file_name)
    students = []

    # Assuming Excel has columns: Name, Grade, ID, Buddy1, Buddy1_Grade, Buddy2, Buddy2_Grade, Buddy3, Buddy3_Grade
    for index, row in df.iterrows():
        student = Student(row['Name'], row['Grade'], row['ID'])
        for buddy_column, buddy_grade_column in zip(['Buddy1', 'Buddy2', 'Buddy3'],
                                                    ['Buddy1_Grade', 'Buddy2_Grade', 'Buddy3_Grade']):
            if pd.notna(row[buddy_column]) and pd.notna(row[buddy_grade_column]):
                student.request_buddy(row[buddy_column], int(row[buddy_grade_column]))
        students.append(student)

    return students

def main():
    file_name = 'testing_data.xlsx'
    students = load_students_from_excel(file_name)

    for student in students:
        assert student.validate_buddy_names(students)

    room_assignments = RoomAssignments(students, 2)
    room_assignments.print(students)

    room_assignments.update_student_info(students)

    # Print final student details
    print_students(students)

if __name__ == "__main__":
    main()
