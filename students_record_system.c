#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // For isalpha, isspace

#define MAX_NAME_LEN 100
#define FILENAME "students.txt"
#define MAX_STUDENTS 100

typedef struct Student {
    char name[MAX_NAME_LEN];
    int roll;
    float marks;
    char result[10];
} Student;

Student *students = NULL;
int student_count = 0;

// Function Prototypes
void save_to_file();
void load_from_file();
void add_student();
void view_all_students();
void search_student();
void delete_student();
void edit_student();
void export_to_csv();
int is_valid_number(char *str);
int is_valid_name(char *str); // Updated prototype for name validation
int is_duplicate(int roll, char *name);
void sort_students_by_marks(int order);
void calculate_average_marks();
int find_student_index_by_name(char *name);
void student_menu_loop(int loggedInStudentIndex);
void admin_menu_loop();

int main() {
    students = (Student *)malloc(sizeof(Student) * MAX_STUDENTS);
    if (students == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return 1;
    }
    load_from_file();

    printf("Welcome to the Student Record System!\n");

    int program_running = 1;
    while(program_running) {
        char role_choice[10];
        printf("\nAre you a student or an admin? (s/a) or (e for Exit Program): ");
        scanf("%s", role_choice);
        getchar(); // consume newline

        if (strcmp(role_choice, "a") == 0 || strcmp(role_choice, "A") == 0) {
            char admin_username[50];
            printf("Enter admin username: ");
            scanf("%s", admin_username);
            getchar(); // consume newline
            if (strcmp(admin_username, "admin") != 0) {
                printf("Access Denied. Incorrect admin username.\n");
            } else {
                printf("Admin login successful.\n");
                admin_menu_loop();
            }
        } else if (strcmp(role_choice, "s") == 0 || strcmp(role_choice, "S") == 0) {
            char student_name_to_login[MAX_NAME_LEN];
            printf("Enter your student name to login: ");
            fgets(student_name_to_login, MAX_NAME_LEN, stdin);
            student_name_to_login[strcspn(student_name_to_login, "\n")] = 0;

            int student_idx = find_student_index_by_name(student_name_to_login);
            if (student_idx != -1) {
                printf("Student login successful. Welcome, %s!\n", students[student_idx].name);
                student_menu_loop(student_idx);
            } else {
                printf("Please put in the correct name to login.\n");
            }
        } else if (strcmp(role_choice, "e") == 0 || strcmp(role_choice, "E") == 0) {
            program_running = 0;
            printf("Goodbye! Thanks for using the Student Record System.\n");
        } else {
            printf("Invalid role choice. Please enter 's', 'a', or 'e'.\n");
        }
    }

    save_to_file();
    free(students);
    return 0;
}

// Admin Menu Loop
void admin_menu_loop() {
    int choice;
    do {
        printf("\n===== ADMIN MENU =====\n");
        printf("1. Add Student\n");
        printf("2. View All Students\n");
        printf("3. Search Student\n");
        printf("4. Delete Student\n");
        printf("5. Edit Student\n");
        printf("6. Export to CSV\n");
        printf("7. Sort by Marks\n");
        printf("8. Calculate Average Marks\n");
        printf("9. Exit Admin Panel\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1: add_student(); break;
            case 2: view_all_students(); break;
            case 3: search_student(); break;
            case 4: delete_student(); break;
            case 5: edit_student(); break;
            case 6: export_to_csv(); break;
            case 7: {
                int sort_order;
                printf("Sort order (1: Ascending, 2: Descending): ");
                scanf("%d", &sort_order);
                getchar();
                if (sort_order == 1) {
                    sort_students_by_marks(1);
                } else if (sort_order == 2) {
                    sort_students_by_marks(2);
                } else {
                    printf("Invalid sort order. Please enter 1 or 2.\n");
                }
                break;
            }
            case 8: calculate_average_marks(); break;
            case 9: printf("Exiting Admin Panel and returning to main menu.\n"); break;
            default: printf("Invalid choice. Please select from the menu.\n");
        }
    } while (choice != 9);
}

// Student Menu Loop
void student_menu_loop(int loggedInStudentIndex) {
    int choice;
    do {
        printf("\n===== STUDENT MENU =====\n");
        printf("1. View My Record\n");
        printf("2. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1:
                if (loggedInStudentIndex != -1) {
                    printf("\n--- Your Record ---\n");
                    printf("Name: %s, Roll: %d, Marks: %.2f, Result: %s\n",
                           students[loggedInStudentIndex].name,
                           students[loggedInStudentIndex].roll,
                           students[loggedInStudentIndex].marks,
                           students[loggedInStudentIndex].result);
                } else {
                    printf("Error: Your record could not be found.\n");
                }
                break;
            case 2:
                printf("Goodbye from student %s's session! Returning to main menu.\n", students[loggedInStudentIndex].name);
                break;
            default:
                printf("Invalid choice. Please select from the menu.\n");
        }
    } while (choice != 2);
}

void add_student() {
    if (student_count >= MAX_STUDENTS) {
        printf("Student limit reached.\n");
        return;
    }

    Student new_student;
    printf("Enter student name: ");
    fgets(new_student.name, MAX_NAME_LEN, stdin);
    new_student.name[strcspn(new_student.name, "\n")] = 0; // Remove newline

    // Validate student name: now only allows letters and spaces
    if (!is_valid_name(new_student.name)) {
        printf("Invalid name. Please use only letters and spaces (no numbers, periods, or other special characters).\n");
        return;
    }

    char roll_str[10];
    printf("Enter roll number: ");
    scanf("%s", roll_str);
    if (!is_valid_number(roll_str)) {
        printf("Invalid roll number. Only numbers allowed.\n");
        getchar();
        return;
    }
    new_student.roll = atoi(roll_str);

    char mark_str[10];
    printf("Enter marks: ");
    scanf("%s", mark_str);
    if (!is_valid_number(mark_str)) {
        printf("Invalid marks. Only numbers allowed.\n");
        getchar();
        return;
    }
    new_student.marks = atof(mark_str);
    getchar(); // Consume newline after scanf for marks

    if (is_duplicate(new_student.roll, new_student.name)) {
        printf("Student with this name and roll already exists.\n");
        return;
    }

    strcpy(new_student.result, new_student.marks >= 40 ? "Pass" : "Fail");
    students[student_count++] = new_student;
    printf("Student added successfully.\n");
    printf("Result for %s: %s\n", new_student.name, new_student.result);
}

void view_all_students() {
    if (student_count == 0) {
        printf("No student records found.\n");
        return;
    }

    printf("\n--- All Student Records ---\n");
    for (int i = 0; i < student_count; i++) {
        printf("Name: %s, Roll: %d, Marks: %.2f, Result: %s\n",
            students[i].name, students[i].roll,
            students[i].marks, students[i].result);
    }
}

void save_to_file() {
    FILE *file = fopen(FILENAME, "w");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }
    for (int i = 0; i < student_count; i++) {
        fprintf(file, "%s,%d,%.2f,%s\n",
            students[i].name, students[i].roll,
            students[i].marks, students[i].result);
    }
    fclose(file);
}

void load_from_file() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        return;
    }
    while (student_count < MAX_STUDENTS && fscanf(file, "%[^,],%d,%f,%[^\n]\n",
        students[student_count].name,
        &students[student_count].roll,
        &students[student_count].marks,
        students[student_count].result) == 4) {
        student_count++;
    }
    fclose(file);
}

void search_student() {
    char query[100];
    printf("Search by (1) Roll or (2) Name: ");
    int opt;
    scanf("%d", &opt);
    getchar();

    if (opt == 1) {
        printf("Enter roll number to search: ");
        scanf("%s", query);
        if (!is_valid_number(query)) {
            printf("Only numbers allowed in roll.\n");
            getchar();
            return;
        }
        int roll = atoi(query);
        for (int i = 0; i < student_count; i++) {
            if (students[i].roll == roll) {
                printf("Found: %s, Roll: %d, Marks: %.2f, Result: %s\n",
                       students[i].name, students[i].roll,
                       students[i].marks, students[i].result);
                return;
            }
        }
    } else if (opt == 2) {
        printf("Enter name to search: ");
        fgets(query, sizeof(query), stdin);
        query[strcspn(query, "\n")] = 0;
        for (int i = 0; i < student_count; i++) {
            if (strcmp(students[i].name, query) == 0) {
                printf("Found: %s, Roll: %d, Marks: %.2f, Result: %s\n",
                       students[i].name, students[i].roll,
                       students[i].marks, students[i].result);
                return;
            }
        }
    } else {
        printf("Invalid search option. Please enter 1 or 2.\n");
    }
    printf("No student record found.\n");
}

void delete_student() {
    char roll_str[10];
    printf("Enter roll number to delete: ");
    scanf("%s", roll_str);
    getchar(); // consume newline
    if (!is_valid_number(roll_str)) {
        printf("Invalid roll number.\n");
        return;
    }
    int roll = atoi(roll_str);
    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll) {
            for (int j = i; j < student_count - 1; j++) {
                students[j] = students[j + 1];
            }
            student_count--;
            printf("Record deleted successfully.\n");
            return;
        }
    }
    printf("No record found for this student.\n");
}

void edit_student() {
    char roll_str[10];
    printf("Enter roll number to edit: ");
    scanf("%s", roll_str);
    getchar(); // consume newline
    if (!is_valid_number(roll_str)) {
        printf("Invalid roll number.\n");
        return;
    }
    int roll = atoi(roll_str);
    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll) {
            printf("Editing %s's record...\n", students[i].name);
            printf("Enter new name: ");
            fgets(students[i].name, MAX_NAME_LEN, stdin);
            students[i].name[strcspn(students[i].name, "\n")] = 0;

            // Validate edited student name: now only allows letters and spaces
            if (!is_valid_name(students[i].name)) {
                printf("Invalid name. Please use only letters and spaces (no numbers, periods, or other special characters). Reverting to old name.\n");
                return;
            }

            char mark_str[10];
            printf("Enter new marks: ");
            scanf("%s", mark_str);
            if (!is_valid_number(mark_str)) {
                printf("Invalid marks.\n");
                getchar();
                return;
            }
            students[i].marks = atof(mark_str);
            getchar(); // consume newline
            strcpy(students[i].result, students[i].marks >= 40 ? "Pass" : "Fail");
            printf("Record updated.\n");
            printf("New result for %s: %s\n", students[i].name, students[i].result);
            return;
        }
    }
    printf("No student record found.\n");
}

void export_to_csv() {
    FILE *f = fopen("students.csv", "w");
    if (!f) {
        perror("Error opening CSV file for writing");
        return;
    }
    fprintf(f, "Name,Roll,Marks,Result\n");
    for (int i = 0; i < student_count; i++) {
        fprintf(f, "%s,%d,%.2f,%s\n",
            students[i].name, students[i].roll,
            students[i].marks, students[i].result);
    }
    fclose(f);
    printf("Exported to students.csv\n");
}

void sort_students_by_marks(int order) {
    if (student_count == 0) {
        printf("No students to sort.\n");
        return;
    }

    for (int i = 0; i < student_count - 1; i++) {
        for (int j = 0; j < student_count - i - 1; j++) {
            if ((order == 1 && students[j].marks > students[j + 1].marks) ||
                (order == 2 && students[j].marks < students[j + 1].marks)) {
                Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }
    if (order == 1) {
        printf("Sorted students by marks (ascending).\n");
    } else {
        printf("Sorted students by marks (descending).\n");
    }
    view_all_students();
}

void calculate_average_marks() {
    if (student_count == 0) {
        printf("No student records to calculate average.\n");
        return;
    }

    float total_marks = 0;
    for (int i = 0; i < student_count; i++) {
        total_marks += students[i].marks;
    }

    float average = total_marks / student_count;
    printf("Average marks for all students: %.2f\n", average);
}

// Checks if a string contains only digits and optionally one decimal point.
int is_valid_number(char *str) {
    int decimal_count = 0;
    if (str[0] == '\0') return 0;
    for (int i = 0; str[i]; i++) {
        if (!isdigit(str[i])) {
            if (str[i] == '.' && i > 0 && str[i+1] != '\0') {
                decimal_count++;
                if (decimal_count > 1) return 0;
            } else {
                return 0;
            }
        }
    }
    return 1;
}

// Modified function: Checks if a string contains only letters or spaces
int is_valid_name(char *str) {
    if (str[0] == '\0') return 0; // Name cannot be empty
    for (int i = 0; str[i] != '\0'; i++) {
        // Allow alphabetic characters or spaces only.
        if (!isalpha(str[i]) && !isspace(str[i])) {
            return 0; // If it's not a letter and not a space, it's invalid.
        }
    }
    return 1;
}

// Checks for duplication based on either roll number or name (OR logic)
int is_duplicate(int roll, char *name) {
    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll || strcmp(students[i].name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

// Finds a student by name and returns their index. Returns -1 if not found.
int find_student_index_by_name(char *name) {
    for (int i = 0; i < student_count; i++) {
        if (strcmp(students[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}