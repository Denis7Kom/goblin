#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MAX_NAME_LEN 50
#define WORK_COOLDOWN 30
#define REST_COOLDOWN 30
#define GOBLIN_LEVEL_TIMER 30
#define GOBLIN_ATTACK_TIMER 75

struct Characters {
    struct Person {
        char name[MAX_NAME_LEN];
        int health;
        float arm;
        float damage;
        int crit_damage;
        int boost_damage;
        int level;
        int gold;
        int rest_count;
        int sleep_bonus;
    } p;

    struct Goblin {
        int health;
        float damage;
        int level;
    } g;
};

// Прототипы функций
void Person_level_up(struct Characters *c);
void Goblin_level_up(struct Characters *c);
void Attack(struct Characters *c);
void Defend(struct Characters *c);
void Rest(struct Characters *c, time_t *last_rest_time);
void Work(struct Characters *c, time_t *last_work_time);
void Shop(struct Characters *c);
void Menu(struct Characters *c, time_t *last_work_time, time_t *last_rest_time, time_t *goblin_attack_time, time_t *goblin_level_time);
void print_ch(struct Characters *c);
void ApplyPromoCode(struct Characters *c);
void Sleep(struct Characters *c);
void Eat(struct Characters *c);

void Person_level_up(struct Characters *c) {
    c->p.health += 25;
    c->p.arm += 2;
    c->p.damage += 0.09f;
    c->p.boost_damage += 4;
    c->p.gold += 50;
    printf("\nCongratulazioni! Il tuo livello è aumentato. Hai guadagnato 50 oro. Oro attuale: %d.\n", c->p.gold);
}

void Goblin_level_up(struct Characters *c) {
    c->g.health += 50;
    c->g.damage += 1.9f;
    printf("\nIl goblin è diventato più forte! Livello: %d, Salute: %d, Danno: %.2f\n", c->g.level, c->g.health, c->g.damage);
}

void Attack(struct Characters *c) {
    int damage_to_goblin = (int)(c->p.damage * c->p.crit_damage + c->p.boost_damage);
    c->g.health -= damage_to_goblin;
    if (c->g.health < 0) c->g.health = 0;

    printf("\n--- Attacco ---\n");
    printf("- %s -\n", c->p.name);
    printf("Livello: %d | Salute: %d | Danno Totale: %.2f | Armatura: %.2f\n", c->p.level, c->p.health, c->p.damage, c->p.arm);
    printf("- Goblin -\n");
    printf("Livello: %d | Salute: %d | Danno Totale: %.2f\n", c->g.level, c->g.health, c->g.damage);

    if (c->g.health > 0) {
        Defend(c);
    }
}

void Defend(struct Characters *c) {
    float damage_received = c->g.damage - 0.3 * c->p.arm;
    if (damage_received > 0) {
        c->p.health -= (int)damage_received;
    }
    if (c->p.health < 0) c->p.health = 0;

    printf("\nIl goblin ti ha attaccato! Hai subito %d danni. Salute attuale: %d.\n", (int)damage_received, c->p.health);
}

void Sleep(struct Characters *c) {
    c->p.sleep_bonus += 2;
    c->p.health += c->p.sleep_bonus;
    printf("\nHai dormito e recuperato %d salute. Salute attuale: %d.\n", c->p.sleep_bonus, c->p.health);
}

void Eat(struct Characters *c) {
    c->p.health += 6;
    printf("\nHai mangiato e recuperato 6 salute. Salute attuale: %d.\n", c->p.health);
}

void Rest(struct Characters *c, time_t *last_rest_time) {
    time_t current_time = time(NULL);
    if (difftime(current_time, *last_rest_time) >= REST_COOLDOWN) {
        int choice;
        printf("\n--- Riposo ---\n1. Dormire\n2. Mangiare\nScegli un'opzione: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                Sleep(c);
                break;
            case 2:
                Eat(c);
                break;
            default:
                printf("\nScelta non valida.\n");
        }
        c->p.rest_count++;
        if (c->p.rest_count % 4 == 0) {
            c->p.level++;
            Person_level_up(c);
        }
        *last_rest_time = current_time;
    } else {
        printf("\nDevi aspettare prima di riposare di nuovo. Prova più tardi.\n");
    }
}

void Work(struct Characters *c, time_t *last_work_time) {
    time_t current_time = time(NULL);
    if (difftime(current_time, *last_work_time) >= WORK_COOLDOWN) {
        c->p.gold += 20;
        if (c->p.rest_count % 3 == 0) {
            c->p.level++;
            Person_level_up(c);
        }
        printf("\nHai lavorato e guadagnato 20 oro! Oro attuale: %d.\n", c->p.gold);
        *last_work_time = current_time;
    } else {
        printf("\nDevi aspettare prima di lavorare di nuovo. Prova più tardi.\n");
    }
}

void ApplyPromoCode(struct Characters *c) {
    char promo[20];
    printf("\nInserisci il codice promozionale: ");
    scanf("%s", promo);

    if (strcmp(promo, "nonna") == 0 || strcmp(promo, "mamma") == 0 || strcmp(promo, "fratello") == 0) {
        c->p.gold += 9999;
        printf("\nHai ricevuto 9999 oro! Oro attuale: %d.\n", c->p.gold);
    } else if (strcmp(promo, "armatura") == 0) {
        c->p.arm += 50;
        printf("\nHai ricevuto 50 armatura! Armatura attuale: %.0f.\n", c->p.arm);
    } else {
        printf("\nCodice promozionale non valido.\n");
    }
}

void Shop(struct Characters *c) {
    printf("\n--- Negozio ---\n");
    printf("1. Pozione di salute (+20 salute) - 30 oro\n");
    printf("2. Miglioramento armatura (+5 armatura) - 50 oro\n");
    printf("3. Miglioramento danno (+2 danno) - 40 oro\n");
    printf("4. Oggetto universale (+5 a tutte le statistiche) - 100 oro\n");
    printf("5. Pozione temporanea (+10 danno per 5 turni) - 80 oro\n");
    printf("6. Uscita\n");

    int choice;
    printf("Scegli un'opzione: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            if (c->p.gold >= 30) {
                c->p.health += 20;
                c->p.gold -= 30;
                printf("\nHai acquistato una pozione di salute! Salute attuale: %d.\n", c->p.health);
            } else {
                printf("\nNon hai abbastanza oro!\n");
            }
            break;
        case 2:
            if (c->p.gold >= 50) {
                c->p.arm += 5;
                c->p.gold -= 50;
                printf("\nHai migliorato la tua armatura! Armatura attuale: %.0f.\n", c->p.arm);
            } else {
                printf("\nNon hai abbastanza oro!\n");
            }
            break;
        case 3:
            if (c->p.gold >= 40) {
                c->p.damage += 2;
                c->p.gold -= 40;
                printf("\nHai migliorato il tuo danno! Danno attuale: %.2f.\n", c->p.damage);
            } else {
                printf("\nNon hai abbastanza oro!\n");
            }
            break;
        case 4:
            if (c->p.gold >= 100) {
                c->p.health += 5;
                c->p.arm += 5;
                c->p.damage += 5;
                c->p.gold -= 100;
                printf("\nHai acquistato un oggetto universale!\n");
            } else {
                printf("\nNon hai abbastanza oro!\n");
            }
            break;
        case 5:
            if (c->p.gold >= 80) {
                c->p.boost_damage += 10;
                c->p.gold -= 80;
                printf("\nHai acquistato una pozione temporanea! Bonus danno: %d.\n", c->p.boost_damage);
            } else {
                printf("\nNon hai abbastanza oro!\n");
            }
            break;
        case 6:
            printf("\nUscita dal negozio.\n");
            break;
        default:
            printf("\nScelta non valida.\n");
    }
}

void Menu(struct Characters *c, time_t *last_work_time, time_t *last_rest_time, time_t *goblin_attack_time, time_t *goblin_level_time) {
    int respond;
    printf("\n--- Menu Principale ---\n");
    printf("0. Caratteristiche\n1. Attaccare\n2. Lavorare\n3. Riposo\n4. Negozio\n5. Codice Promozionale\n");
    printf("Scegli un'opzione: ");
    scanf("%d", &respond);

    switch (respond) {
        case 0:
            print_ch(c);
            break;
        case 1:
            Attack(c);
            break;
        case 2:
            Work(c, last_work_time);
            break;
        case 3:
            Rest(c, last_rest_time);
            break;
        case 4:
            Shop(c);
            break;
        case 5:
            ApplyPromoCode(c);
            break;
        default:
            printf("\nScelta non valida.\n");
    }
}

void print_ch(struct Characters *c) {
    printf("\n--- Caratteristiche ---\n");
    printf("- %s -\n", c->p.name);
    printf("Livello: %d\nSalute: %d\nArmatura: %.0f\nDanno: %.2f\nDanno critico: %d\nDanno aggiuntivo: %d\nOro: %d\n",
           c->p.level, c->p.health, c->p.arm, c->p.damage, c->p.crit_damage, c->p.boost_damage, c->p.gold);
    printf("- Goblin -\n");
    printf("Livello: %d\nSalute: %d\nDanno: %.2f\n", c->g.level, c->g.health, c->g.damage);
}

int main(void) {
    struct Characters c = {
        .p = {"Eroe", 60, 5, 13.2f, 1, 0, 1, 100, 0, 2},
        .g = {150, 9.0f, 1}
    };

    time_t last_work_time = 0;
    time_t last_rest_time = 0;
    time_t goblin_attack_time = time(NULL);
    time_t goblin_level_time = time(NULL);

    printf("\nInserisci il nome del tuo personaggio: ");
    scanf("%s", c.p.name);

    print_ch(&c);

    while (c.p.health > 0 && c.g.health > 0) {
        time_t current_time = time(NULL);

        if (difftime(current_time, goblin_attack_time) >= GOBLIN_ATTACK_TIMER) {
            Defend(&c);
            goblin_attack_time = current_time;
        }

        if (difftime(current_time, goblin_level_time) >= GOBLIN_LEVEL_TIMER) {
            c.g.level++;
            Goblin_level_up(&c);
            goblin_level_time = current_time;
        }

        Menu(&c, &last_work_time, &last_rest_time, &goblin_attack_time, &goblin_level_time);
    }

    if (c.p.health <= 0) {
        printf("\nHai perso! Il goblin ti ha sconfitto.\n");
    } else if (c.g.health <= 0) {
        printf("\nCongratulazioni! Hai sconfitto il goblin.\n");
    }

    return 0;
}
