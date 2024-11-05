import csv
import random
import time
from typing import List, Dict

# Constants
K_INVALID_ID = -1
K_BEDS_PER_ROOM = 4
K_MAX_BUDDY_COUNT = 3
GRADE_PENALTY = 500  # Penalty for being two or more grades apart
TIME_LIMIT = 30  # Time in seconds to search for optimal room assignment


class Student:
    def __init__(self, name: str, grade: int, student_id: int):
        self.name = name
        self.grade = grade
        self.student_id = student_id
        self.room_number = K_INVALID_ID
        self.buddies = []
        self.buddy_ids = []

    def request_buddy(self, buddy_name: str) -> bool:
        if len(self.buddies) < K_MAX_BUDDY_COUNT and buddy_name not in self.buddies:
            self.buddies.append(buddy_name)
            return True
        return False

    def is_buddy_of(self, other_id: int) -> bool:
        return other_id in self.buddy_ids

    def validate_buddy_names(self, all_students: Dict[int, 'Student']) -> bool:
        for buddy_name in self.buddies:
            found = False
            for student in all_students.values():
                if student.name == buddy_name:
                    found = True
                    self.buddy_ids.append(student.student_id)
                    break
            if not found:
                print(f"Student {self.name}'s buddy {buddy_name} not found.")
                return False
        return True

    def calculate_score(self, all_students: Dict[int, 'Student'], roommates: List[int]) -> int:
        score = 0
        buddies_in_room = sum(1 for other_id in roommates if self.is_buddy_of(other_id))

        for other_id in roommates:
            if other_id == self.student_id:
                continue
            grade_gap = abs(self.grade - all_students[other_id].grade)
            if grade_gap >= 2:
                score -= GRADE_PENALTY

        # Add points for each buddy in the room and subtract for missing buddies
        # score += buddies_in_room * 100 - (len(self.buddy_ids) - buddies_in_room) * 50
        score += buddies_in_room * 100
        return score


class Room:
    def __init__(self, room_id: int):
        self.room_id = room_id
        self.students = []

    def assign_student(self, student_id: int) -> bool:
        if len(self.students) < K_BEDS_PER_ROOM:
            self.students.append(student_id)
            return True
        return False

    def calculate_score(self, all_students: Dict[int, Student]) -> int:
        return sum(all_students[student_id].calculate_score(all_students, self.students) for student_id in self.students)


class RoomAssignments:
    def __init__(self, all_students: Dict[int, Student], room_count: int):
        self.rooms = [Room(room_id) for room_id in range(room_count)]
        self.all_students = all_students
        self.score = -float("inf")
        self.generate_random_assignment()

    def generate_random_assignment(self):
        unassigned_students = list(self.all_students.keys())
        random.shuffle(unassigned_students)

        for room in self.rooms:
            room.students.clear()

        for student_id in unassigned_students:
            for room in self.rooms:
                if room.assign_student(student_id):
                    break

        self.calculate_total_score()

    def calculate_total_score(self):
        self.score = sum(room.calculate_score(self.all_students) for room in self.rooms)

    def update_room_numbers(self):
        for room in self.rooms:
            for student_id in room.students:
                self.all_students[student_id].room_number = room.room_id


class DecaConference:
    def __init__(self, file_path: str):
        self.all_students = {}
        self.room_count = 0
        self.load_data(file_path)

    def load_data(self, file_path: str):
        with open(file_path, 'r') as file:
            reader = csv.reader(file)
            self.student_count = int(next(reader)[0])
            self.room_count = int(self.student_count + K_BEDS_PER_ROOM - 1) // K_BEDS_PER_ROOM

            for row in reader:
                student_id = int(row[2])
                student = Student(name=row[1], grade=int(row[3]), student_id=student_id)
                self.all_students[student_id] = student

                for buddy_name in row[5:5 + int(row[4])]:  # Adjust for the buddy count
                    student.request_buddy(buddy_name)

            for student in self.all_students.values():
                student.validate_buddy_names(self.all_students)

    def optimize_room_assignment(self):
        start_time = time.time()
        best_assignment = RoomAssignments(self.all_students, self.room_count)

        while time.time() - start_time < TIME_LIMIT:
            current_assignment = RoomAssignments(self.all_students, self.room_count)
            if current_assignment.score > best_assignment.score:
                best_assignment = current_assignment
                print(f"Found new max score: {best_assignment.score}")

        best_assignment.update_room_numbers()
        self.print_room_assignments(best_assignment)

    def print_room_assignments(self, best_assignment: RoomAssignments):
        print(f"Total score for all rooms: {best_assignment.score}")
        for room in best_assignment.rooms:
            print(f"Room {room.room_id} Score: {room.calculate_score(self.all_students)}")
            students_names = [self.all_students[s_id].name for s_id in room.students]
            print(f" Students: {', '.join(students_names)}")


if __name__ == "__main__":
    deca_conference = DecaConference("old_data.csv")
    deca_conference.optimize_room_assignment()
