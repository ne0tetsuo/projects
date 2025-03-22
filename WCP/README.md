# Wikicomptine Protocol (WCP) Project

## Overview
The Wikicomptine Protocol (WCP) project implements a client-server application in C, designed for sharing and managing nursery rhymes (comptines) through TCP/IP sockets. This project provides functionalities for downloading, uploading, listing, and selecting nursery rhymes in a networked environment.

## Features
- **Client-Server Communication**: Robust TCP/IP communication between client and server.
- **Multithreading**: Concurrent handling of multiple client requests using pthreads.
- **File Management**: Upload and download nursery rhymes files (`.cpt`) with proper format validation.
- **Logging System**: Records all server interactions (requests, downloads, uploads) along with client IP, timestamps, and requested comptines.
- **Error Handling**: Comprehensive error checking for socket connections, file operations, and memory allocation.

## Project Structure
```
├── comptine_utils.c
├── comptine_utils.h
├── main.c
├── wcp_clt.c
├── wcp_srv.c
├── wcp_srv2.c
├── Makefile
└── comptines/ (directory containing .cpt files)
```

## Compilation and Execution

### Compile
```bash
make
```

### Run Server
```bash
./wcp_srv <comptines_directory> [max_clients]
```
**Example:**
```bash
./wcp_srv ./comptines 10
```

### Run Client
```bash
./wcp_clt <server_ip_address>
```
**Example:**
```bash
./wcp_clt 127.0.0.1
```

## Usage Instructions

### Client Commands
- `1`: Choose and display a comptine from the server.
- `2`: Upload a comptine to the server.
- `3`: List available comptines on the server.
- `4`: Exit the client.

## Requirements
- GCC Compiler
- POSIX-compliant Operating System (e.g., Linux)

## Project Author
- **Mohammed Yanis TAKBOU**  
  ID: `12206365`

This project represents original work, entirely designed and implemented by Mohammed Yanis TAKBOU.


