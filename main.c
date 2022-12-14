#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#define RED     "\x1B[31m"
#define GRN     "\x1B[32m"
#define YEL     "\x1B[33m"
#define BLU     "\x1B[34m"
#define MAG     "\x1B[35m"
#define CYN     "\x1B[36m"
#define WHT     "\x1B[37m"
#define RESET   "\x1B[0m"

#define STORAGE_DIR "/tmp/remind_storage"
#define LISTFILE "/tmp/remind_storage/remind.txt"
#define MAX_DESCRIPTION_SIZE 102
#define MAX_REMINDERS 50


struct reminder {
    char description[MAX_DESCRIPTION_SIZE];
};

struct reminder reminders[MAX_REMINDERS] = {{0}}; 

int directory_exists(char *path) {
    struct stat stats;

    stat(path, &stats);

    // Check for file existence
    if (S_ISDIR(stats.st_mode))
        return 1;

    return 0;
};

void check_make_dir(char *path) {
    if (directory_exists(STORAGE_DIR)) {
    } else {
        mkdir(STORAGE_DIR, S_IRWXU);
    };
};

int save_storage(FILE *fp) {

    check_make_dir(STORAGE_DIR); 

    fp = fopen(LISTFILE, "w");
    if (fp == NULL) {
        perror(RED "Error opening file" RESET);
        return(-1);
    };

    int counter = 0;
    
    while(1) {
        if (counter == MAX_REMINDERS) {
            break;
        };
        if (strncmp(reminders[counter].description, "", 1) == 0) {
            break;
        };   
        fputs(reminders[counter].description, fp);
        counter++;
    };
    fclose(fp);
    return(0);

};

int load_storage(FILE *fp) {
    
    fp = fopen(LISTFILE, "r");
    if (fp == NULL) {
        perror(RED "Error opening file" RESET);
        return(-1);
    };
    int counter = 0;
    while (1) {
        if (counter == MAX_REMINDERS) {
            break;
        }
        fgets(reminders[counter].description, MAX_DESCRIPTION_SIZE, (FILE*)fp);
        if ( feof(fp) ) {
            break;
        }
        counter++;
    }
    fclose(fp);
    return(0);
    
};

int main(int argc, char *argv[]){
    FILE *fp;
    char buff[MAX_DESCRIPTION_SIZE];

    if (strncmp(argv[1], "show", 4) == 0){
        
        int lf = load_storage(fp);
        if (lf == -1) {
            return (-1);
        }

        printf(CYN "REMINDERS -----------------------------------------------\n" RESET);

        int counter = 0;
        while (counter < MAX_REMINDERS) {
            if (strncmp(reminders[counter].description, "", 1) == 0) {
                break;
            }
            printf("%i: %s", counter + 1, reminders[counter].description);
            counter++;
        }

    } else if (strncmp(argv[1], "add", 3) == 0) { 
     
        int lf = load_storage(fp);
        if (lf == -1) {
            save_storage(fp);
        }

        printf(MAG "Enter description for new item (Limit %i): " RESET, MAX_DESCRIPTION_SIZE - 2);
        fgets(buff, MAX_DESCRIPTION_SIZE, stdin);
        
        int counter = 0;
        int found_new_line = 0;
        while (counter < MAX_DESCRIPTION_SIZE) {
            if (strncmp(&buff[counter], "\n", 2) == 0) {
                found_new_line = 1;
                break;
            };
            if (counter == (MAX_DESCRIPTION_SIZE - 1) && found_new_line == 0) {
                // swap end char with new line char
                strncpy(&buff[MAX_DESCRIPTION_SIZE - 2], "\n", 2);
            };
            counter++;
        };
        
        counter = 0;
        while (counter < MAX_REMINDERS) {
            if (strncmp(reminders[counter].description, "", 1) == 0) {
                strncpy(reminders[counter].description, buff, MAX_DESCRIPTION_SIZE);
                save_storage(fp);
                printf(GRN "Item successfully added to reminders list.\n" RESET);
                break;
            };
            counter++;
        }

    } else if (strncmp(argv[1], "rm", 2) == 0) {
        
        int input = atoi(argv[2]);

        if ((0 < input ) && (input <= MAX_REMINDERS)) {
            load_storage(fp);
            if (strncmp(reminders[input-1].description, "", 1) == 0) {
                fprintf(stderr, RED "The reminder you tried to remove does not exist.\n" RESET);
                return(-1);     
            } else {
                int counter = input-1; 
                while(1) {
                    if (counter == MAX_REMINDERS) {
                        break;
                    };
                    reminders[counter] = reminders[counter + 1];
                    counter++;
                };
            };
        } else {
            
            fprintf(stderr, RED "The reminder you tried to remove does not exist.\n" RESET);
            return(-1);

        };
        printf(GRN "Successfully removed item %i.\n" RESET, input);
        save_storage(fp);

    } else if (strncmp(argv[1], "dl", 2) == 0) {  

        remove(LISTFILE);
        printf("List successfully deleted. \n" RESET);

    } else if (strncmp(argv[1], "db", 2) == 0) {
       
        printf(GRN "Useful command for debugging.\n" RESET);
        // USE THIS TO DEBUG

    } else {

        fprintf(stderr, "Invalid arg passed.\n");
        return(-1);

    }

    return 0;
}

