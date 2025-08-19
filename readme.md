# Contact Management System

This is a simple console-based contact management system developed in C. It allows users to register, log in, manage their contacts, and organize them into groups. The data is persisted to local text files.

## Features

* **User Authentication:**
    * **Register:** Create a new user account.
    * **Login:** Access the system with an existing account.

* **Contact Management:**
    * **Add Contact:** Store a new contact with details such as name, phone number, email, and address.
    * **Show Contacts:** View a list of all saved contacts.
    * **Search Contact:** Find a specific contact by name or phone number.
    * **Update Contact:** Modify the details of an existing contact.
    * **Delete Contact:** Remove a contact from the list.
    * **Sort Contacts:** Sort contacts alphabetically by name.

* **Group Management:**
    * **Create Group:** Organize contacts into groups.
    * **Show Groups:** View existing groups and their members.

## How to Compile and Run

This project consists of multiple C source files and header files.

### Compilation

To compile the project, you need a C compiler like GCC. Use the following command in your terminal:

```sh
gcc main.c auth.c contact.c groups.c -o contact_manager
```

* `main.c`: Contains the main application logic and user menu.
* `auth.c`: Handles user registration and login.
* `contact.c`: Implements contact-related functions (add, show, search, etc.).
* `groups.c`: Manages group creation and display.
* `-o contact_manager`: Specifies the output executable file name.

### Running the Application

After successful compilation, you can run the executable:

```sh
./contact_manager
```

## File Structure

* `main.c`: The entry point of the program.
* `auth.c`, `auth.h`: User authentication logic.
* `contact.c`, `contact.h`: Contact management logic.
* `groups.c`, `groups.h`: Group management logic.
* `customers.txt`: Stores user credentials.
* `contacts.txt`: Stores contact data.

**Note:** The `.txt` files are created automatically by the program to store data.