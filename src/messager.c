#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_USER 32
#define DEFAULT_USER "root"
#define MAX_LEN 1024
#define PASS_MIN 8
#define PASS_MAX 24

struct User {
    int is_admin;
    int is_active;
    char* name;
    char* passwd;
};
struct Message {
    char* message;
    struct Message *next;
};
int nb_user = 0;
struct User* users[MAX_USER] = {0};
struct Message *messages = NULL;

void malloc_fail() {
    perror("Malloc fail, exit ...");
    exit(1);
}

char* bool_to_string(int i) {
    if (i) {
        return "True";
    } else {
        return "False";
    }
}

char* get_input(char* buffarg, char* prompt) {
    char* buff = buffarg;
    int len;

    if (buff == NULL) {
        buff = (char*) malloc(MAX_LEN + 1);

        if (buff == NULL) {
            malloc_fail();
        }
    }

    printf(prompt);
    fgets(buff, MAX_LEN+1, stdin);
    len = strlen(buff);
    buff[len] = '\0';
    if (buff[len-1] == '\n'){
        buff[len-1] = '\0';
        len -=1;
    }

    return buff;
}

int get_integer() {
    char buff[MAX_LEN+1];
    get_input(buff, "");

    return atoi(buff);
}


void init_prog() {
    char* passwd = NULL; 
    struct User* firstuser = NULL;

    passwd = getenv ("DEFAULT_PASSWD");
    if (passwd == NULL) {
        perror("No default password for root");
        exit(1);
    }

    firstuser = (struct User*) malloc(sizeof(struct User));
    if (firstuser == NULL) {
        malloc_fail();
    }

    firstuser->name = malloc(sizeof(char) * (strlen(DEFAULT_USER) + 1)); 
    firstuser->passwd = malloc(sizeof(char) * (strlen(passwd) + 1));
    strncpy(firstuser->name, DEFAULT_USER, strlen(DEFAULT_USER) + 1);
    strncpy(firstuser->passwd, passwd, strlen(passwd) + 1);
    firstuser->is_admin = 1;
    firstuser->is_active = 0;

    users[0] = firstuser;
    nb_user = 1;

    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
}

int search_user(char* buff) {
    int i;
    for (i=0; i<nb_user; i++) {
        if (!strcmp(buff, users[i]->name)) {
            return i;
        }
    }
    return -1;
}

int change_passwd(struct User* user) {
    char* buff = NULL;
    buff = get_input(NULL, "Enter password : ");

    if (strlen(buff) < PASS_MIN) {
        printf("Too small password.\n");
        free(buff);
        return 1;
    } else if (strlen(buff) > PASS_MAX) {
        printf("Too long password.\n");
        free(buff);
        return 1;
    }

    if (user->passwd != NULL) {
        free(user->passwd);
    }
    user->passwd = buff;
    return 0;
}

int create_new_user() {
    char* buff;
    struct User* new_user;

    if (nb_user == MAX_USER) {
        printf("No more user can be registered.\n");
        return 1;
    }

    new_user = malloc(sizeof(struct User));

    if (new_user == NULL) {
        malloc_fail();
    }

    buff = get_input(NULL, "Enter username : ");

    if (search_user(buff) >= 0){
        printf("Username already use\n.");
        free(buff);
        free(new_user);
        return 1;
    }

    new_user->name = buff;
    new_user->passwd = NULL;
    new_user->is_admin = 0;
    new_user->is_active = 0;

    if (change_passwd(new_user)) {
        free(buff);
        free(new_user);
        return 1;
    }

    users[nb_user] = new_user;

    nb_user += 1;

    printf("User created : %s\n", new_user->name);
    return 0;
}

void list_user() {
    int user;
    printf("\nList Users :\n"
           "[active] | [Username]\n");
    for (user = 0; user < nb_user; user ++) {
        printf("  %5s  | %s\n", bool_to_string(users[user]->is_active), users[user]->name);
    }
}

void change_user_admin() {

    char buff[MAX_LEN + 1] = {0};
    char *buffer = NULL;
    int user, choice;
    
    get_input(buff, "Enter username to modify : ");

    user = search_user(buff);
    if (user < 0 || user >= nb_user) {
        printf("Not found user\n");
        return;
    }
    
    printf("User info : \n"
           "  username : %s\n"
           "  is_active : %s\n"
           "  is_admin : %s\n\n",
           users[user]->name,
           bool_to_string(users[user]->is_active), 
           bool_to_string(users[user]->is_admin) 
        );
    printf("Actions : \n"
           " 0 : Exit\n"
           " 1 : Change username\n"
           " 2 : Change password\n");
    if (users[user]->is_active) {
        printf(" 3 : Set user inactive\n");
    } else {
        printf(" 3 : Set user active\n");
    }

    choice = -1;
    while (choice < 0 || choice > 3) {
        printf("> ");
        choice = get_integer();
    }
    
    switch (choice) {
        case 1:
            buffer = get_input(NULL, "Enter username : ");

            if (search_user(buffer) >= 0){
                printf("Username already use.\n");
                free(buffer);
                return ;
            }

            free(users[user]->name);
            users[user]->name = buffer;
            break;
        case 2:
            change_passwd(users[user]);
            break;
        case 3:
            users[user]->is_active = !users[user]->is_active;
            break;
    }
}

void remove_user(int user) {
    free(users[user]->passwd);
    free(users[user]->name);
    free(users[user]);
    nb_user -= 1;
    users[user] = users[nb_user];
    printf("User deleted\n");
}

void remove_user_admin() {

    char buff[MAX_LEN +1] = {0};
    int user;
    
    get_input(buff, "Enter username to delete : ");

    user = search_user(buff);
    if (user < 0 || user >= nb_user) {
        printf("Not found user\n");
    } else {
        remove_user(user);
    }
}

void show_my_data(struct User* user) {
    printf("Username : %s\n"
           "Password : %s\n\n",
           user->name,
           user->passwd);
}

void list_messages() {
    int count = 0;
    struct Message *m;
    m = messages;
    if (m == NULL) {
        printf("No message\n");
    } else {
        printf("Messages : \n");
        while (m != NULL) {
            count +=1;
            printf("[%d] : %s\n", count, m->message);
            m = m->next;
        }
    }
}

void add_message() {
    struct Message *m;
    struct Message *new_m;

    new_m = (struct Message*) malloc(sizeof(struct Message));
    new_m->message = get_input(NULL, "Enter your message : ");
    new_m->next = (struct Message*) NULL;

    if (messages == NULL) {
        messages = new_m;
    } else {
        m = messages;
        while (m->next != NULL) {
            m = m->next;
        }
        m->next = new_m;
    }
}

void menu(int user_id) {
    struct User* user = users[user_id];
    int choice;

    while(1) {
        printf("\n\n");
        if (users[user_id]->is_admin) {
            printf("Actions :\n"
                   " 1 : create new user\n"
                   " 2 : list users\n"
                   " 3 : manage users\n"
                   " 4 : delete account\n"
                   " 0 : Logout\n> ");
            choice = get_integer();
            printf("\n\n");
            switch (choice) {
                case 1:
                    if (!create_new_user()) {
                        users[nb_user-1]->is_active = 1;
                    }
                    break;
                case 2:
                    list_user();
                    break;
                case 3: 
                    change_user_admin();
                    break;
                case 4:
                    remove_user_admin();
                    break;
                case 0:
                    return;
                default:
                    printf("Unknow options.\n");
            }
        } else if (users[user_id]->is_active) {
            printf("Actions :\n"
                   " 1 : show messages\n"
                   " 2 : add message\n"
                   " 3 : show my data\n"
                   " 4 : change password\n"
                   " 5 : delete account\n"
                   " 0 : Logout\n> ");
            choice = get_integer();
            printf("\n\n");
            switch (choice) {
                case 1:
                    list_messages();
                    break;
                case 2:
                    add_message();
                    break;
                case 3:
                    show_my_data(user);
                    break;
                case 4: 
                    change_passwd(user);
                    break;
                case 5:
                    remove_user(user_id);
                case 0:
                    return;
                default:
                    printf("Unknow options.\n");
            }
        } else {
            printf("Your accounts was not activate.\n"
                   "Please contact an admin to activate it\n"
                   "Actions :\n"
                   " 1 : change password\n"
                   " 2 : delete account\n"
                   " 0 : Logout\n> ");
            choice = get_integer();
            printf("\n\n");
            switch (choice) {
                case 1: 
                    change_passwd(user);
                    break;
                case 2:
                    remove_user(user_id);
                case 0:
                    return;
                default:
                    printf("Unknow options.\n");
            }
        }
    }
}


void login() {
    int user;
    char buff[MAX_LEN];
    char passwd_to_verify[MAX_LEN];

    get_input(buff, "Username : ");
    user = search_user(buff);
    if (user < 0 || user >= nb_user || (users[user]->is_admin && !users[user]->is_active)) {
        printf("Not found user\n");
        return;
    }
    strcpy(passwd_to_verify, users[user]->passwd);

    get_input(buff, "Password : ");

    if (!strncmp(buff, passwd_to_verify, PASS_MAX)) {
        printf("Success login with %s\n", users[user]->name);
        menu(user);
    } else {
        printf("Invalid password.\n");
    }
}

int main() {
    int choice;
    init_prog();
    printf("Welcome in messager128 (v 1.0r1)\n");

    while (1) {
        choice = -1;

        printf("\n\n"
               "Please choose an option between the follow : \n"
               " 1 : create an user\n"
               " 2 : login\n"
               " 0 : exit\n"
               "> ");
        choice = get_integer();
        printf("\n\n");

        switch (choice) {
            case 0:
                printf("Good bye\n");
                exit(0);
                break;
            case 1:
                create_new_user();
                break;
            case 2:
                login();
                break;
            default:
                printf("Unknow options.\n");
        }
    }
}
