#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "contacts.dat"

// Structure to store contact details
typedef struct {
    int id;
    char name[50];
    char phone[15];
    char email[50];
} Contact;

// Function prototypes
void addContact();
void displayContacts();
void searchContact();
void editContact();
void deleteContact();
int getNextId();

int main() {
    int choice;

    while (1) {
        printf("\n===== Contact Management System =====\n");
        printf("1. Add Contact\n");
        printf("2. Display All Contacts\n");
        printf("3. Search Contact\n");
        printf("4. Edit Contact\n");
        printf("5. Delete Contact\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear newline from buffer

        switch (choice) {
            case 1: addContact(); break;
            case 2: displayContacts(); break;
            case 3: searchContact(); break;
            case 4: editContact(); break;
            case 5: deleteContact(); break;
            case 6: 
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
    return 0;
}

// Generate next unique ID
int getNextId() {
    FILE *fp = fopen(FILE_NAME, "rb");
    Contact temp;
    int maxId = 0;

    if (fp == NULL)
        return 1;

    while (fread(&temp, sizeof(Contact), 1, fp)) {
        if (temp.id > maxId)
            maxId = temp.id;
    }
    fclose(fp);
    return maxId + 1;
}

// Add a new contact
void addContact() {
    FILE *fp = fopen(FILE_NAME, "ab");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    Contact *newContact = (Contact *)malloc(sizeof(Contact));
    if (newContact == NULL) {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return;
    }

    newContact->id = getNextId();

    printf("Enter Name: ");
    fgets(newContact->name, sizeof(newContact->name), stdin);
    newContact->name[strcspn(newContact->name, "\n")] = '\0';

    printf("Enter Phone: ");
    fgets(newContact->phone, sizeof(newContact->phone), stdin);
    newContact->phone[strcspn(newContact->phone, "\n")] = '\0';

    printf("Enter Email: ");
    fgets(newContact->email, sizeof(newContact->email), stdin);
    newContact->email[strcspn(newContact->email, "\n")] = '\0';

    fwrite(newContact, sizeof(Contact), 1, fp);
    printf("Contact added successfully with ID %d!\n", newContact->id);

    free(newContact);
    fclose(fp);
}

// Display all contacts
void displayContacts() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("No contacts found.\n");
        return;
    }

    Contact *temp = (Contact *)malloc(sizeof(Contact));
    if (temp == NULL) {
        printf("Memory allocation failed!\n");
        fclose(fp);
        return;
    }

    printf("\n--- Contact List ---\n");
    while (fread(temp, sizeof(Contact), 1, fp)) {
        printf("ID: %d\nName: %s\nPhone: %s\nEmail: %s\n",
               temp->id, temp->name, temp->phone, temp->email);
        printf("---------------------------\n");
    }

    free(temp);
    fclose(fp);
}

// Search contact by name
void searchContact() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("No contacts found.\n");
        return;
    }

    char name[50];
    int found = 0;

    printf("Enter name to search: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    Contact temp;
    while (fread(&temp, sizeof(Contact), 1, fp)) {
        if (strstr(temp.name, name) != NULL) {
            printf("\nContact Found:\n");
            printf("ID: %d\nName: %s\nPhone: %s\nEmail: %s\n",
                   temp.id, temp.name, temp.phone, temp.email);
            found = 1;
        }
    }

    if (!found)
        printf("Contact not found.\n");

    fclose(fp);
}

// Edit a contact by ID
void editContact() {
    FILE *fp = fopen(FILE_NAME, "rb+");
    if (fp == NULL) {
        printf("No contacts found.\n");
        return;
    }

    int id, found = 0;
    Contact temp;

    printf("Enter Contact ID to edit: ");
    scanf("%d", &id);
    getchar();

    while (fread(&temp, sizeof(Contact), 1, fp)) {
        if (temp.id == id) {
            printf("Enter new Name: ");
            fgets(temp.name, sizeof(temp.name), stdin);
            temp.name[strcspn(temp.name, "\n")] = '\0';

            printf("Enter new Phone: ");
            fgets(temp.phone, sizeof(temp.phone), stdin);
            temp.phone[strcspn(temp.phone, "\n")] = '\0';

            printf("Enter new Email: ");
            fgets(temp.email, sizeof(temp.email), stdin);
            temp.email[strcspn(temp.email, "\n")] = '\0';

            fseek(fp, -sizeof(Contact), SEEK_CUR);
            fwrite(&temp, sizeof(Contact), 1, fp);
            printf("Contact updated successfully!\n");
            found = 1;
            break;
        }
    }

    if (!found)
        printf("Contact with ID %d not found.\n", id);

    fclose(fp);
}

// Delete a contact by ID
void deleteContact() {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (fp == NULL) {
        printf("No contacts found.\n");
        return;
    }

    FILE *tempFile = fopen("temp.dat", "wb");
    if (tempFile == NULL) {
        printf("Error creating temporary file!\n");
        fclose(fp);
        return;
    }

    int id, found = 0;
    Contact temp;

    printf("Enter Contact ID to delete: ");
    scanf("%d", &id);
    getchar();

    while (fread(&temp, sizeof(Contact), 1, fp)) {
        if (temp.id == id) {
            found = 1;
            continue; // Skip writing this record
        }
        fwrite(&temp, sizeof(Contact), 1, tempFile);
    }

    fclose(fp);
    fclose(tempFile);

    remove(FILE_NAME);
    rename("temp.dat", FILE_NAME);

    if (found)
        printf("Contact deleted successfully!\n");
    else
        printf("Contact with ID %d not found.\n", id);
}
