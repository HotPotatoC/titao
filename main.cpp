#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct TitaoGame
{
    unsigned int max_rounds;
    int board[3][3];
    const char *leaderboard_file_path;
} titao_game = {
    .max_rounds = 5,
    .board = {
        {0, 0, 0},
        {0, 0, 0},
        {0, 0, 0},
    },
    .leaderboard_file_path = "leaderboard.txt",
};

typedef struct Player
{
    char name[101];
    long int win_count, lost_count, draw_count;
} player_t;

player_t player_one = {
    .win_count = 0,
    .lost_count = 0,
    .draw_count = 0,
};

player_t player_two = {
    .win_count = 0,
    .lost_count = 0,
    .draw_count = 0,
};

void menu_play();
void menu_leaderboard();
void menu_exit();

char parse_player(int xo);
int check_winner();

void board_draw();
void board_reset();
int board_position_is_empty(int x, int y);
int board_insert_at(int row, int column, int value);

void reset_players();

int main()
{
    int option;
    do
    {
        puts("\t\t\t*~*~*~*~*~* Titao *~*~*~*~*~*~");
        puts("\t\t\t~         Main Menu          *");
        puts("\t\t\t*  1. Play                   ~");
        puts("\t\t\t~  2. Leaderboard            *");
        puts("\t\t\t*  3. Exit                   ~");
        puts("\t\t\t~                            *");
        puts("\t\t\t*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~\n\n\n");
        printf("Select menu: ");
        scanf("%d", &option);
        puts("");
        switch (option)
        {
        case 1:
            menu_play();
            break;
        case 2:
            menu_leaderboard();
            break;
        case 3:
            menu_exit();
            break;
        }
    } while (option != 3);
    return 0;
}

void menu_play()
{
    int rounds;
    // system("cls||clear");

input_player_one:
    printf("Input nama player 1 (X): ");
    scanf("%s", player_one.name);

    if (strlen(player_one.name) > 100)
    {
        puts("Nama player 1 terlalu panjang (max: 100)");
        goto input_player_one;
    }

input_player_two:
    printf("Input nama player 2 (O): ");
    scanf("%s", player_two.name);

    if (strlen(player_two.name) > 100)
    {
        puts("Nama player 2 terlalu panjang (max: 100)");
        goto input_player_two;
    }

input_round:
    printf("Jumlah Ronde (max 5): ");
    scanf("%d", &rounds);

    if (rounds > titao_game.max_rounds)
    {
        printf("! Maksimal ronde hanya %d !\n", titao_game.max_rounds);
        goto input_round;
    }

    for (int current_round = 1; current_round <= rounds; current_round++)
    {
        int winner = 0;
        int current_move = 1;
        printf("Round %d\n", current_round);

        int player_one_round_score = 0;
        int player_two_round_score = 0;
        do
        {
        reset_round:
            winner = check_winner();

            board_draw();
            if (winner != 0)
            {
                char current_player[255];
                if (winner == -1)
                {
                    player_one_round_score++;
                    strcpy(current_player, player_one.name);
                }
                else
                {
                    player_two_round_score++;
                    strcpy(current_player, player_two.name);
                }

                // system("cls||clear");
                printf("==== %s Menang Ronde %d ====\n\n", current_player, current_round);
                break;
            }

            int position;
            printf("Input posisi (1-9): ");
            scanf("%d", &position);

            if (position == 1 || position == 2 || position == 3)
            {
                if (board_insert_at(0, position - 1, current_move % 2 != 0 ? -1 : 1) == -1)
                {
                    puts("Posisi sudah terisi");
                    goto reset_round;
                }
            }
            else if (position == 4 || position == 5 || position == 6)
            {
                if (board_insert_at(1, position - 4, current_move % 2 != 0 ? -1 : 1) == -1)
                {
                    puts("Posisi sudah terisi");
                    goto reset_round;
                }
            }
            else if (position == 7 || position == 8 || position == 9)
            {
                if (board_insert_at(2, position - 7, current_move % 2 != 0 ? -1 : 1) == -1)
                {
                    puts("Posisi sudah terisi");
                    goto reset_round;
                }
            }
            else
            {
                puts("Invalid Position (1-9)");
            }

            if (current_move % 2 != 0)
            {
                current_move++;
            }
            else
            {
                current_move--;
            }
        } while (winner == 0);

        board_reset();
        if (player_one_round_score > player_two_round_score)
        {
            player_one.win_count++;
            player_two.lost_count++;
        }
        else if (player_one_round_score < player_two_round_score)
        {
            player_two.win_count++;
            player_one.lost_count++;
        }
        else
        {
            player_one.draw_count++;
            player_two.draw_count++;
        }
    }

    // Save to leaderboards
    FILE *leaderboards_fp = fopen(titao_game.leaderboard_file_path, "a+");

    // Determines if the file is empty https://stackoverflow.com/a/13566193/12472142
    // If the file is empty then we can assume that it is the first time the user is playing
    // and we can write the player's name and score to the file
    if (fgetc(leaderboards_fp) == EOF)
    {
        puts("EMPTY");
        fprintf(leaderboards_fp, "%s#%ld#%ld#%ld\n", player_one.name, player_one.win_count, player_one.lost_count, player_one.draw_count);
        fprintf(leaderboards_fp, "%s#%ld#%ld#%ld\n", player_two.name, player_two.win_count, player_two.lost_count, player_two.draw_count);
        fclose(leaderboards_fp);
        return;
    }

    unsigned int lines = 0;

    // Get amount of lines in a file https://www.geeksforgeeks.org/c-program-count-number-lines-file/
    for (char tmp_c = getc(leaderboards_fp); tmp_c != EOF; tmp_c = getc(leaderboards_fp))
    {
        // Increment lines if the current character is newline
        if (tmp_c == '\n')
        {
            lines++;
        }
    }

    player_t *leaderboard_players = (player_t *)malloc(lines * sizeof(player_t));
    if (leaderboard_players == NULL)
    {
        puts("Error allocating memory");
        return;
    }

    // Rewind the file pointer to the beginning of the file
    rewind(leaderboards_fp);

    // Set all values to 0
    memset(leaderboard_players, 0, sizeof(player_t) * lines);

    int i = 0;
    while (fscanf(leaderboards_fp, "%[^#]#%ld#%ld#%ld\n",
                  leaderboard_players[i].name, &leaderboard_players[i].win_count,
                  &leaderboard_players[i].lost_count, &leaderboard_players[i].draw_count) != EOF)
    {
        i++;
    }

    for (int i = 0; i < lines; i++)
    {
        if (strcmp(player_one.name, leaderboard_players[i].name) == 0)
        {
            leaderboard_players[i].win_count += player_one.win_count;
            leaderboard_players[i].lost_count += player_one.lost_count;
            leaderboard_players[i].draw_count += player_one.draw_count;
        }
        else if (strcmp(player_two.name, leaderboard_players[i].name) == 0)
        {
            leaderboard_players[i].win_count += player_two.win_count;
            leaderboard_players[i].lost_count += player_two.lost_count;
            leaderboard_players[i].draw_count += player_two.draw_count;
        }
    }

    // Increase the leaderboard_players array size by 2
    player_t *new_leaderboard_players = (player_t *)realloc(leaderboard_players, (lines + 2) * sizeof(player_t));
    if (new_leaderboard_players == NULL)
    {
        puts("Error allocating memory");
        return;
    }

    // Copy the old leaderboard_players array to the new one
    memcpy(new_leaderboard_players, leaderboard_players, sizeof(player_t) * lines + 2);

    // Insert the new players to the new leaderboard
    new_leaderboard_players[lines].win_count = player_one.win_count;
    new_leaderboard_players[lines].lost_count = player_one.lost_count;
    new_leaderboard_players[lines].draw_count = player_one.draw_count;
    strcpy(new_leaderboard_players[lines].name, player_one.name);

    new_leaderboard_players[lines + 1].win_count = player_two.win_count;
    new_leaderboard_players[lines + 1].lost_count = player_two.lost_count;
    new_leaderboard_players[lines + 1].draw_count = player_two.draw_count;
    strcpy(new_leaderboard_players[lines + 1].name, player_two.name);

    // print the new leaderboard
    for (int i = 0; i < lines + 2; i++)
    {
        printf("%s#%ld#%ld#%ld\n", new_leaderboard_players[i].name, new_leaderboard_players[i].win_count, new_leaderboard_players[i].lost_count, new_leaderboard_players[i].draw_count);
    }

    // Rewrite the file with the new leaderboard
    for (int i = 0; i < lines + 2; i++)
    {
        fprintf(leaderboards_fp, "%s#%ld#%ld#%ld\n",
                new_leaderboard_players[i].name, new_leaderboard_players[i].win_count,
                new_leaderboard_players[i].lost_count, new_leaderboard_players[i].draw_count);
    }

    free(new_leaderboard_players);
    // truncate the file to the 0
    fclose(leaderboards_fp);

    // Reset player one and player two
    reset_players();
}

void menu_leaderboard()
{
    puts("\t\t\t*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~");
    puts("\t\t\t~   Ini menu leaderboard!   *");
    puts("\t\t\t*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~");
}

void menu_exit()
{
    puts("\t\t\t*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*");
    puts("\t\t\t* Terima Kasih Sudah Bermain! ~");
    puts("\t\t\t*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*");
}

void board_draw()
{
    printf("\n\n\tSelamat Bermain!\n\n");

    printf("%s (X)  -  %s (O)\n\n\n", player_one.name, player_two.name);

    printf("___________________\n");

    printf("|     |     |     |\n");
    printf("|  %c  |  %c  |  %c  |\n", parse_player(titao_game.board[0][0]), parse_player(titao_game.board[0][1]), parse_player(titao_game.board[0][2]));

    printf("|_____|_____|_____|\n");
    printf("|     |     |     |\n");

    printf("|  %c  |  %c  |  %c  |\n", parse_player(titao_game.board[1][0]), parse_player(titao_game.board[1][1]), parse_player(titao_game.board[1][2]));

    printf("|_____|_____|_____|\n");
    printf("|     |     |     |\n");

    printf("|  %c  |  %c  |  %c  |\n", parse_player(titao_game.board[2][0]), parse_player(titao_game.board[2][1]), parse_player(titao_game.board[2][2]));

    printf("|_____|_____|_____|\n\n");
}

void board_reset()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            titao_game.board[i][j] = 0;
        }
    }
}

char parse_player(int xo)
{
    if (xo == -1)
        return 'X';
    if (xo == 1)
        return 'O';
    return ' ';
}

int check_winner()
{
    for (int i = 0; i < 3; i++) /* check baris */
        if (titao_game.board[i][0] == titao_game.board[i][1] &&
            titao_game.board[i][0] == titao_game.board[i][2])
            return titao_game.board[i][0];

    for (int i = 0; i < 3; i++) /* check kolom */
        if (titao_game.board[0][i] == titao_game.board[1][i] &&
            titao_game.board[0][i] == titao_game.board[2][i])
            return titao_game.board[0][i];

    /* Check diagonal dari kiri atas ke kanan bawah */
    if (titao_game.board[0][0] == titao_game.board[1][1] &&
        titao_game.board[1][1] == titao_game.board[2][2])
        return titao_game.board[0][0];

    /* Check diagonal dari kanan atas ke kiri bawah */
    if (titao_game.board[0][2] == titao_game.board[1][1] &&
        titao_game.board[1][1] == titao_game.board[2][0])
        return titao_game.board[0][2];

    return 0;
}

int board_position_is_empty(int row, int column)
{
    return titao_game.board[row][column] == 0;
}

int board_insert_at(int row, int column, int value)
{
    printf("Value: %d\n", value);
    if (!board_position_is_empty(row, column))
    {
        return -1;
    }

    titao_game.board[row][column] = value;
    return 1;
}

int board_is_full()
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (titao_game.board[i][j] == 0)
                return -1;
        }
    }
    return 1;
}

void reset_players()
{
    memset(&player_one, 0, sizeof(player_t));
    memset(&player_two, 0, sizeof(player_t));
}