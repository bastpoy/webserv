# WEBSERV

## Comprehension des commandes

- epoll : permet de gerer chaque fd concernant chaque requete
    
    struct epoll_event {
        uint32_t      events;  /* Epoll events */
        epoll_data_t  data;    /* User data variable */
    };

    union epoll_data {
        void     *ptr;
        int       fd;
        uint32_t  u32;
        uint64_t  u64;
    };


## NOTION HTTP

- Comprendre la RFC et implementer les differentes fonctionnalites de cette norme


### FORMAT DE BASE

Ligne de début (start-line)
En-têtes (headers)
Ligne vide (CRLF)
Corps du message (body) optionnel

### REQUETE

GET / HTTTP/1.1

premier argument: methode (post ou get)
deuxieme argument: chemin apres mon nom de domaine
troisieme argument: version HTTP

**En-Tetes requete**

Host (obligatoire)
User-Agent
Accept
Accept-Language
Accept-Encoding
Authorization

### REPONSE


**En-Tetes reponses**

Server
Content-Type
Content-Length
Content-Encoding
Location

### CODE ERREUR ET SUCCES

1xx : Information
2xx : Succès
3xx : Redirection
4xx : Erreur Client
5xx : Erreur Serveur

## FONCTIONNALITES A IMPLEMENTER

- Repertoire de fichiers statiques
- Redirection HTTP
- Accepter des fichiers

## CGI

### FONCTIONNALITES

- Implementer un CGI suivant l'extension du fichier demande.
- Le faire fonctionner avec GET et POST.

### EXPLICATIONS

- C'est une option sur un serveur qui va permettre de gerer des requetes specifiques comme des passages de script en parametre

## TACHES

- Comprendre la RFC sur http et les en-tetes requete et reponses
- Comment implementer epoll pour le multiplexing
- Commencer par gerer les simples requetes
- Decouper mes requetes en examinant le header de la requete
- Pouvoir GET POST DELETE
- Comprendre le CGI et comment l'implemeneter
- Setup routes http?
- Activer, desactiver le listing des repertoires?

## PARSING

- parsing a partir du fichier de configuration.
- un bloc http 
    => un nombre de server dans ce http
        => redirection
        => location
        => root
- class: 
        - http
        - server
        - location

- dans http: 
        => server(plusieurs)
- dans server:
        => listen (port)
        => server_name 
        => root
        => index
        => return (redirection)
        => location
- dans location:
        => index
        => return (redirection)
    