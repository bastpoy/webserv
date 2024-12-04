#ifndef GLOBALLINKLIST_HPP
#define GLOBALLINKLIST_HPP

#include "Header.hpp"

// Basic Node Structure
struct Node {
    t_serverData *data;
    Node* next;
    
    // Constructor
    Node(t_serverData *val) : data(val), next(NULL) {}
};

// Global Linked List Class
class GlobalLinkedList {
private:
    static Node* head;
    static Node* tail;

public:
    static void insert(t_serverData *data) 
    {
        Node* newNode = new Node(data);
        // std::cout << "adding " << data << std::endl;
        if (head == NULL) {
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
    }
    
    static void print() 
    {
        Node* current = head;
        while (current != NULL) {
            std::cout << current->data << " ";
            current = current->next;
        }
        std::cout << std::endl;
    }

    static void update_data(t_serverData *data) 
    {
        Node *copy = head;
        if(!head)
            return;
        while(head != NULL)
        {
            if(head->data == data)
            {
                head->data = NULL;
            }
            head = head->next;
        }
        head = copy;
    }

    static void cleanup() 
    {
        while (head != NULL) {
            Node* temp = head;
            head = head->next;
            if(temp->data)
            {
                if(temp->data->cgi)
                {
                    delete temp->data->cgi;
                    temp->data->cgi = NULL;
                }
                delete temp->data;
                temp->data = NULL;
            }
            delete temp;
        }
        tail = NULL;
    }
};

#endif