# Contact Management System

A contact management system written in **C** with authentication, contact management, group management, file-based data storage, and a graphical interface.

## Features

- User registration and login

- Add contacts

- View contacts

- Search contacts

- Update contacts

- Delete contacts

- Sort contacts alphabetically

- Create and manage groups

- Persistent data storage

- Graphical user interface

- Backend server support

- Windows and Linux GUI launch scripts

## Technologies

- C

- Python

- HTML / CSS / JavaScript

- GCC

- Make

- Git

- GitHub

## Project Structure

```
contact-management/
├── main.c
├── auth.c
├── auth.h
├── contact.c
├── contact.h
├── groups.c
├── groups.h
├── contacts.txt
├── customers.txt
├── groups.txt
├── Makefile
├── server.py
├── run_gui.bat
├── run_gui.sh
├── data/
├── gui/
└── README.md
```

# Overview

The project is divided into multiple modules.

# Authentication

Handles:

- User registration
- User login
- User account management
- Contact Management

  # Handles:

- Adding contacts
- Viewing contacts
- Searching contacts
- Updating contacts
- Deleting contacts
- Sorting contacts
- Group Management
- Creating groups
- Viewing groups
- Organizing contacts into groups

# Compilation
Using GCC
gcc main.c auth.c contact.c groups.c -o contact_manager

Run:
./contact_manager

# Using Makefile
-make

# GUI

The project also includes a graphical interface.

The GUI provides a visual way to interact with the contact management system.

# Linux
./run_gui.sh

# Windows
run_gui.bat

# Data Storage

The application stores data locally using text files.

- customers.txt — User information
- contacts.txt — Contact information
- groups.txt — Group information
- data/ — Additional application data

# Backend

The project includes server.py, which provides backend/server functionality for the graphical interface.

# File Description
- main.c — Main application logic and menu
- auth.c — Authentication implementation
- auth.h — Authentication declarations
- contact.c — Contact management implementation
- contact.h — Contact management declarations
- groups.c — Group management implementation
- groups.h — Group management declarations
- server.py — Backend server
- Makefile — Build automation
- run_gui.sh — Linux GUI launcher
- run_gui.bat — Windows GUI launcher
- gui/ — Graphical interface files

# Learning Objectives

This project helps understand:

- C programming
- File handling
- Structures and modular programming
- Authentication logic
- Contact and data management
- Group management
- GUI development
- Backend integration
- Build automation
- Future Improvements
- Database integration
- Improved authentication
- Better GUI design
- Contact import and export
- Advanced search and filtering
- Cloud synchronization
- Improved security

# Author

Shivank Garg

# GitHub:

https://github.com/Shivank-Garg45
