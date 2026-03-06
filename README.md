# C++ Learning Project

Personal project created to experiment with C++ and systems used in multiplayer games.

The repository is public mainly to showcase the code and development process.

## Purpose

This project is used to learn and experiment with:

* C++ programming
* client-server networking
* packet based communication
* SDL window creation and rendering
* basic encryption

The goal is not to create a finished game, but to understand how these systems work internally.

## Architecture

The project follows a simple client-server structure.

* **Client**

    * SDL window and input
    * networking
    * packet handling

* **Server**

    * player connection management
    * packet processing
    * command handling

* **Shared**

    * structures and code used by both client and server

## Technologies

* C++
* SDL3
* ENet
* Crypto++
* yaml-cpp

## Notes

The codebase may change frequently as the project evolves since it is primarily used for learning and experimentation.
