#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define GRAVITY 9.81
#define TOLERANCE 1.0
#define MAX_IMPACT 1.0

typedef struct
{
    int id;
    char type[3];

    double x;
    double y;

    double vMin;
    double vMax;

    double angleMin;
    double angleMax;
    double angleRange;

    double impactPower;

    int alive;
    int hasFired;
} EscortShip;

typedef struct
{
    char type[3];

    double x;
    double y;

    double initialX;
    double initialY;

    double vMax;
} Battleship;

typedef struct
{
    double x;
    double y;
} Point;

//random number
double randomBetween(double min, double max)
{
    return min + ((double)rand() / RAND_MAX) * (max - min);
}


//Escort Shop properties

void setEscortProperties(EscortShip *E)
{
    if (strcmp(E->type, "EA") == 0)
    {
        E->impactPower = 0.08;
        E->angleRange = 20.0;
    }
    else if (strcmp(E->type, "EB") == 0)
    {
        E->impactPower = 0.06;
        E->angleRange = 30.0;
    }
    else if (strcmp(E->type, "EC") == 0)
    {
        E->impactPower = 0.07;
        E->angleRange = 25.0;
    }
    else if (strcmp(E->type, "ED") == 0)
    {
        E->impactPower = 0.05;
        E->angleRange = 50.0;
    }
    else
    {
        strcpy(E->type, "EE");
        E->impactPower = 0.04;
        E->angleRange = 70.0;
    }
}

//generate escort ships

void generateEscortShips(EscortShip E[],
                         int N,
                         double D,
                         double battleshipVmax)
{
    char escortTypes[5][3] =
    {
        "EA", "EB", "EC", "ED", "EE"
    };

    for (int i = 0; i < N; i++)
    {
        E[i].id = i + 1;

        E[i].x = randomBetween(0.0, D);
        E[i].y = randomBetween(0.0, D);

        int typeIndex = rand() % 5;

        strcpy(E[i].type, escortTypes[typeIndex]);

        setEscortProperties(&E[i]);

        if (strcmp(E[i].type, "EA") == 0)
        {
            E[i].vMax = 1.2 * battleshipVmax;
        }
        else
        {
            E[i].vMax = randomBetween(0.0, battleshipVmax);
        }

        E[i].vMin = randomBetween(0.0, E[i].vMax);

        E[i].angleMin = randomBetween(0.0,90.0 - E[i].angleRange);

        E[i].angleMax = E[i].angleMin + E[i].angleRange;

        E[i].alive = 1;
        E[i].hasFired = 0;
    }
}

void copyEscortShips(EscortShip destination[],
                     EscortShip source[],
                     int N)
{
    for (int i = 0; i < N; i++)
    {
        destination[i] = source[i];
    }
}


//Generate Battle path
void generatePath(Point path[], int k, double D)
{
    for (int i = 0; i < k; i++)
    {
        path[i].x = randomBetween(0.0, D);

        path[i].y = randomBetween(0.0, D);
    }
}


//Display Path
void displayPath(Point path[], int k)
{
    printf("\n========================================\n");
    printf("BATTLESHIP PATH\n");
    printf("========================================\n");

    for (int i = 0; i < k; i++)
    {
        printf("Point %d : (%.2f, %.2f)\n",
               i + 1,
               path[i].x,
               path[i].y);
    }
}


//E Escort Ships attack Battle ship
int escortAttackBattleshipC(
        Battleship *B,
        EscortShip E[],
        int N,
        double *cumulativeImpact,
        int *sinkingID,
        double *sinkingTime)
{
    double firingVelocity;
    double firingAngle;
    double angleRadians;

    double dx;
    double dy;

    double flightTime;
    double projectileY;

    for (int i = 0; i < N; i++)
    {
        if (E[i].alive == 0)
        {
            continue;
        }
        if (E[i].hasFired == 1)
        {
            continue;
        }

        firingVelocity = randomBetween(E[i].vMin,E[i].vMax);

        firingAngle = randomBetween (E[i].angleMin,E[i].angleMax);

        angleRadians = firingAngle * M_PI / 180.0;

        dx = fabs(B->x - E[i].x);

        dy = B->y - E[i].y;

        printf("\nE%d attacks Battleship\n",E[i].id);
        printf("  Firing Velocity : %.2f m/s\n",firingVelocity);
        printf("  Firing Angle    : %.2f degrees\n",firingAngle);

        E[i].hasFired = 1;

        if (firingVelocity <= 0.0)
        {
            printf("  Result          : MISS\n");
            continue;
        }


        //Same vertical position
        if (dx < 0.000001)
        {
            double maximumHeight =
                (firingVelocity *
                 firingVelocity *
                 sin(angleRadians) *
                 sin(angleRadians))
                /
                (2.0 * GRAVITY);

            if (dy >= 0.0 &&
                dy <= maximumHeight)
            {
                flightTime =dy /(firingVelocity * sin(angleRadians));

                printf("  Result          : HIT\n");

                printf("  Time to Hit     : %.2f seconds\n",flightTime);

                *cumulativeImpact += E[i].impactPower;

                if (*cumulativeImpact > MAX_IMPACT)
                {
                    *cumulativeImpact = MAX_IMPACT;
                }

                printf("  Impact Power    : %.2f%%\n",E[i].impactPower * 100.0);

                printf("  Cumulative B Damage : %.2f%%\n", *cumulativeImpact * 100.0);

                if (*cumulativeImpact >= MAX_IMPACT)
                {
                    *sinkingID = E[i].id;
                    *sinkingTime = flightTime;

                    return 1;
                }
            }
            else
            {
                printf("  Result          : MISS\n");
            }

            continue;
        }


        //Projectile cannot travel backwards
        if (cos(angleRadians) <= 0.0)
        {
            printf("  Result          : MISS\n");
            continue;
        }


        //Time required to reach B horizontally
        flightTime = dx /(firingVelocity * cos(angleRadians));


        projectileY =
            E[i].y +
            firingVelocity *
            sin(angleRadians) *
            flightTime -
            0.5 *
            GRAVITY *
            flightTime *
            flightTime;

        printf("  Projectile Y    : %.2f m\n",projectileY);

        printf("  Battleship Y    : %.2f m\n",B->y);


        if (fabs(projectileY - B->y) <= TOLERANCE)
        {
            printf("  Result          : HIT\n");

            printf("  Time to Hit     : %.2f seconds\n",flightTime);

            *cumulativeImpact += E[i].impactPower;

            if (*cumulativeImpact > MAX_IMPACT)
            {
                *cumulativeImpact = MAX_IMPACT;
            }

            printf("  Impact Power    : %.2f%%\n", E[i].impactPower * 100.0);

            printf("  Cumulative B Damage : %.2f%%\n",*cumulativeImpact * 100.0);


            if (*cumulativeImpact >= MAX_IMPACT)
            {
                *sinkingID = E[i].id;
                *sinkingTime = flightTime;

                return 1;
            }
        }
        else
        {
            printf("  Result          : MISS\n");
        }
    }

    return 0;
}

//Battle Ship attacks Escort ships

int battleshipAttackEscortsC(
        Battleship *B,
        EscortShip E[],
        int N,
        double gunMinAngle,
        double *battleEndTime)
{
    double firingVelocity;

    double dx;
    double dy;

    double discriminant;

    double lowerAngle;
    double upperAngle;

    double angleRadians;

    double flightTime;
    double projectileY;

    int hitCount = 0;

    firingVelocity = B->vMax;

    *battleEndTime = 0.0;

    printf("\n========================================\n");
    printf("BATTLESHIP ATTACKS ESCORT SHIPS\n");
    printf("========================================\n");

    for (int i = 0; i < N; i++)
    {
        if (E[i].alive == 0)
        {
            continue;
        }

        dx = fabs(E[i].x - B->x);

        dy = E[i].y - B->y;

        printf("\nE%d:\n", E[i].id);


        //same vertical line
        if (dx < 0.000001)
        {
            if (90.0 < gunMinAngle)
            {
                printf("  Result          : CANNOT REACH\n");
                continue;
            }

            if (dy >= 0.0 &&
                dy <=
                (firingVelocity *
                 firingVelocity)
                /
                (2.0 * GRAVITY))
            {
                flightTime = dy / firingVelocity;

                printf("  Firing Velocity : %.2f m/s\n",firingVelocity);

                printf("  Firing Angle    : 90.00 degrees\n");

                printf("  Result          : HIT\n");

                hitCount++;

                E[i].alive = 0;

                if (flightTime > *battleEndTime)
                {
                    *battleEndTime = flightTime;
                }
            }
            else
            {
                printf("  Result          : CANNOT REACH\n");
            }

            continue;
        }


        //Projectile equation

        discriminant = pow(firingVelocity, 4.0)
            -
            GRAVITY *
            (
                GRAVITY * dx * dx
                +
                2.0 * dy *
                firingVelocity *
                firingVelocity
            );


        if (discriminant < 0.0)
        {
            printf("  Result          : CANNOT REACH\n");
            continue;
        }

        //lower firing angle
        lowerAngle =
            atan(
                (
                    firingVelocity *
                    firingVelocity
                    -
                    sqrt(discriminant)
                )
                /
                (GRAVITY * dx)
            );

        //Upper firing angle

        upperAngle =
            atan(
                (
                    firingVelocity *
                    firingVelocity
                    +
                    sqrt(discriminant)
                )
                /
                (GRAVITY * dx)
            );


        angleRadians = -1.0;

        if (lowerAngle >=
                gunMinAngle *
                M_PI / 180.0 &&
            lowerAngle <=
                M_PI / 2.0)
        {
            angleRadians =
                lowerAngle;
        }

        else if (upperAngle >=
                    gunMinAngle *
                    M_PI / 180.0 &&
                 upperAngle <=
                    M_PI / 2.0)
        {
            angleRadians = upperAngle;
        }


        if (angleRadians < 0.0)
        {
            printf("  Result          : CANNOT REACH\n");
            continue;
        }


        flightTime = dx / (firingVelocity * cos(angleRadians));


        projectileY =
            B->y +
            firingVelocity *
            sin(angleRadians) *
            flightTime -
            0.5 *
            GRAVITY *
            flightTime *
            flightTime;


        printf("  Firing Velocity : %.2f m/s\n",firingVelocity);

        printf("  Firing Angle    : %.2f degrees\n",angleRadians * 180.0 / M_PI);

        printf("  Target          : (%.2f, %.2f)\n",E[i].x,E[i].y);

        printf("  Projectile Y    : %.2f m\n",projectileY);

        if (fabs(projectileY - E[i].y) <= TOLERANCE)
        {
            printf("  Result          : HIT\n");

            hitCount++;

            E[i].alive = 0;

            if (flightTime > *battleEndTime)
            {
                *battleEndTime = flightTime;
            }
        }
        else
        {
            printf("  Result          : MISS\n");
        }
    }

    return hitCount;
}

//saving initial conditions

void saveInitialConditionsC(
        const char *filename,
        double D,
        int N,
        Battleship B,
        EscortShip E[],
        int k,
        Point path[])
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error creating %s\n",filename);
        return;
    }


    fprintf(file,"========================================\n");

    fprintf(file,"ADVANCED NAVAL BATTLE SIMULATOR\n");

    fprintf(file,"PART 1-C - INITIAL CONDITIONS\n");

    fprintf(file,"========================================\n\n");


    fprintf(file,"BATTLEFIELD\n");

    fprintf(file,"Size: %.2f x %.2f\n",D, D);

    fprintf(file, "Number of Escort Ships: %d\n\n",N);


    fprintf(file,"BATTLESHIP\n");

    fprintf(file,"Type: %s\n",B.type);

    fprintf(file,"Initial Position: (%.2f, %.2f)\n",B.initialX,B.initialY);

    fprintf(file,"Vmax: %.2f m/s\n\n",B.vMax);


    fprintf(file,"ESCORT SHIPS\n\n");


    for (int i = 0; i < N; i++)
    {
        fprintf(file,"E%d\n",E[i].id);

        fprintf(file,"Type: %s\n",E[i].type);

        fprintf(file,"Position: (%.2f, %.2f)\n",E[i].x, E[i].y);

        fprintf(file,"Vmin: %.2f m/s\n",E[i].vMin);

        fprintf(file,"Vmax: %.2f m/s\n",E[i].vMax);

        fprintf(file,"Minimum Angle: %.2f degrees\n",E[i].angleMin);

        fprintf(file,"Maximum Angle: %.2f degrees\n",E[i].angleMax);

        fprintf(file,"Angle Range: %.2f degrees\n",E[i].angleRange);

        fprintf(file,"Impact Power: %.2f%%\n",E[i].impactPower * 100.0);

        fprintf(file,"\n");
    }


    if (k > 0)
    {
        fprintf(file,"BATTLESHIP PATH\n\n");

        fprintf(file,"Number of Path Points: %d\n\n",k);

        for (int i = 0; i < k; i++)
        {
            fprintf(file,"Point %d: (%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
        }
    }


    fprintf(file,"\nGravity: %.2f m/s^2\n",GRAVITY);

    fprintf(file,"Initial Cumulative Impact on B: 0.00%%\n");

    fclose(file);
}

//save Part1-C result

void saveResultC(
        const char *filename,
        int simulationNumber,
        int iteration,
        Battleship B,
        EscortShip E[],
        int N,
        int battleshipSunk,
        int sinkingID,
        double sinkingTime,
        int hitCount,
        double battleEndTime,
        Point currentPoint,
        int gunJammed,
        double thetaMin,
        double cumulativeImpact)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error creating %s\n",filename);
        return;
    }


    fprintf(file,"========================================\n");

    fprintf(file,"ADVANCED NAVAL BATTLE SIMULATOR\n");

    fprintf(file,"PART 1-C\n");

    fprintf(file,"SIMULATION %d - ITERATION %d\n",simulationNumber,iteration);

    fprintf(file,"========================================\n\n");


    fprintf(file, "BATTLESHIP POSITION\n");

    fprintf(file,"(%.2f, %.2f)\n\n",currentPoint.x,currentPoint.y);


    fprintf(file,"GUN STATUS\n");


    if (gunJammed)
    {
        fprintf(file,"JAMMED\n");

        fprintf(file,"Allowed Vertical Angle: %.2f - 90.00 degrees\n\n",thetaMin);
    }
    else
    {
        fprintf(file,"NORMAL\n");

        fprintf(file,"Allowed Vertical Angle: 0.00 - 90.00 degrees\n\n");
    }


    fprintf(file,"BATTLE RESULT\n");


    if (battleshipSunk)
    {
        fprintf(file,"Battleship Status: SUNK\n");

        fprintf(file,"Escort Ship that completed the destruction: E%d\n",sinkingID);

        fprintf(file,"Time of Final Impact: %.2f seconds\n",sinkingTime);

        fprintf(file,"Cumulative Impact on B: 100.00%%\n\n");
    }
    else
    {
        fprintf(file,"Battleship Status: SURVIVED\n");

        fprintf(file,"Number of Escort Ships Hit by B: %d\n",hitCount);

        fprintf(file,"Battle End Time: %.2f seconds\n",battleEndTime);

        fprintf(file,"Cumulative Impact on B: %.2f%%\n\n",cumulativeImpact * 100.0);
    }


    fprintf(file,"FINAL BATTLESHIP CONDITIONS\n");

    fprintf(file,"Type: %s\n",B.type);

    fprintf(file,"Position: (%.2f, %.2f)\n",B.x,B.y);

    fprintf(file, "Vmax: %.2f m/s\n",B.vMax);

    fprintf(file, "Cumulative Damage: %.2f%%\n\n",cumulativeImpact * 100.0);


    fprintf(file,"ESCORT SHIP CONDITIONS\n\n");


    for (int i = 0; i < N; i++)
    {
        fprintf(file,"E%d\n",E[i].id);

        fprintf(file,"Type: %s\n",E[i].type);

        fprintf(file,"Position: (%.2f, %.2f)\n",E[i].x,E[i].y);

        fprintf(file,"Status: %s\n",E[i].alive ?"ALIVE":"DESTROYED");

        fprintf(file,"Has Fired: %s\n",E[i].hasFired ?"YES" :"NO");

        fprintf(file,"Vmin: %.2f m/s\n",E[i].vMin);

        fprintf(file,"Vmax: %.2f m/s\n",E[i].vMax);

        fprintf(file,"Minimum Angle: %.2f degrees\n",E[i].angleMin);

        fprintf(file,"Maximum Angle: %.2f degrees\n",E[i].angleMax);

        fprintf(file,"Impact Power: %.2f%%\n\n",E[i].impactPower * 100.0);
    }


    fclose(file);
}


//Part 1-C: fixed battle simulation

void runFixedSimulationC(Battleship BStart,EscortShip initialE[],int N)
{
    Battleship B = BStart;
    EscortShip E[N];

    copyEscortShips(E,initialE,N);
    
    double cumulativeImpact = 0.0;
    int sinkingID = -1;
    double sinkingTime = 0.0;


    Point currentPoint;

    currentPoint.x = B.x;
    currentPoint.y = B.y;


    printf("\n\n");
    printf("################################################\n");
    printf("PART 1-C FIXED BATTLEFIELD SIMULATION\n");
    printf("################################################\n");

    //Escort ships attack Battle ship first

    int battleshipSunk =
        escortAttackBattleshipC(
            &B,
            E,
            N,
            &cumulativeImpact,
            &sinkingID,
            &sinkingTime
        );


    if (battleshipSunk)
    {
        printf("\n***************************************\n");
        printf("BATTLESHIP DESTROYED\n");
        printf("Escort Ship: E%d\n",sinkingID);

        printf("Time of Final Impact: %.2f seconds\n",sinkingTime);

        printf("Cumulative Impact: 100.00%%\n");

        printf("***************************************\n");


        saveResultC(
            "part1_C_fixed_final.txt",
            1,
            1,
            B,
            E,
            N,
            1,
            sinkingID,
            sinkingTime,
            0,
            0.0,
            currentPoint,
            0,
            0.0,
            cumulativeImpact
        );

        return;
    }
    
    //Battle ship attacks Escort ships.

    double battleEndTime = 0.0;

    int hitCount =
        battleshipAttackEscortsC(
            &B,
            E,
            N,
            0.0,
            &battleEndTime
        );


    printf("\n========================================\n");
    printf("FIXED SIMULATION RESULT\n");
    printf("========================================\n");

    printf("Battleship survived.\n");

    printf("Escort Ships destroyed by B: %d\n",hitCount);

    printf("Cumulative Impact on B: %.2f%%\n",cumulativeImpact * 100.0);


    saveResultC(
        "part1_C_fixed_final.txt",
        1,
        1,
        B,
        E,
        N,
        0,
        -1,
        0.0,
        hitCount,
        battleEndTime,
        currentPoint,
        0,
        0.0,
        cumulativeImpact
    );
}


//runpath simulation

void runPathSimulationC(
        int simulationNumber,
        Battleship BStart,
        EscortShip initialE[],
        int N,
        Point path[],
        int k,
        int jamIteration,
        double thetaMin)
{
    Battleship B = BStart;

    EscortShip E[N];

    copyEscortShips(
        E,
        initialE,
        N
    );


    double cumulativeImpact = 0.0;


    printf("\n\n");
    printf("################################################\n");
    printf("SIMULATION %d STARTED\n",simulationNumber);
    printf("################################################\n");


    for (int iteration = 1;
         iteration <= k;
         iteration++)
    {

        B.x = path[iteration - 1].x;
        B.y = path[iteration - 1].y;


        printf("\n\n========================================\n");

        printf("SIMULATION %d - ITERATION %d\n",simulationNumber,iteration);

        printf("========================================\n");


        printf("Battleship Position: (%.2f, %.2f)\n",B.x,B.y);

        int gunJammed = 0;

        double gunMinimumAngle = 0.0;


        if (simulationNumber == 2 && iteration > jamIteration)
        {
            gunJammed = 1;

            gunMinimumAngle = thetaMin;
        }


        if (gunJammed)
        {
            printf("Gun Status: JAMMED\n");

            printf("Allowed Vertical Angle: %.2f - 90.00 degrees\n",thetaMin);
        }
        else
        {
            printf("Gun Status: NORMAL\n");

            printf("Allowed Vertical Angle: 0.00 - 90.00 degrees\n");
        }

        int sinkingID = -1;

        double sinkingTime = 0.0;


        int battleshipSunk =
            escortAttackBattleshipC(
                &B,
                E,
                N,
                &cumulativeImpact,
                &sinkingID,
                &sinkingTime
            );

        if (battleshipSunk)
        {
            printf("\n***************************************\n");

            printf("BATTLESHIP DESTROYED\n");

            printf("Escort Ship: E%d\n",sinkingID);

            printf("Time of Final Impact: %.2f seconds\n",sinkingTime);

            printf("Cumulative Impact: 100.00%%\n");

            printf("Simulation stopped.\n");

            printf("***************************************\n");


            char filename[100];

            sprintf(filename,"part1_C_sim%d_iteration%d.txt",simulationNumber,iteration);
            
            saveResultC(
                filename,
                simulationNumber,
                iteration,
                B,
                E,
                N,
                1,
                sinkingID,
                sinkingTime,
                0,
                0.0,
                path[iteration - 1],
                gunJammed,
                thetaMin,
                cumulativeImpact
            );


            break;
        }

        double battleEndTime = 0.0;
        
        int hitCount =
            battleshipAttackEscortsC(
                &B,
                E,
                N,
                gunMinimumAngle,
                &battleEndTime
            );
        
        printf("\n----------------------------------------\n");

        printf("ITERATION %d RESULT\n",iteration);

        printf("----------------------------------------\n");
        printf("Battleship survived.\n");

        printf("Escort Ships destroyed by B: %d\n",hitCount);

        printf("Cumulative Impact on B: %.2f%%\n",cumulativeImpact * 100.0);

        printf("Battle End Time: %.2f seconds\n",battleEndTime);

        char filename[100];

        sprintf(filename,"part1_C_sim%d_iteration%d.txt",simulationNumber,iteration);
        
        saveResultC(
            filename,
            simulationNumber,
            iteration,
            B,
            E,
            N,
            0,
            -1,
            0.0,
            hitCount,
            battleEndTime,
            path[iteration - 1],
            gunJammed,
            thetaMin,
            cumulativeImpact
        );
    }


    printf("\n========================================\n");

    printf("SIMULATION %d COMPLETED\n",simulationNumber);

    printf("========================================\n");

    printf("Final cumulative impact on B: %.2f%%\n",cumulativeImpact * 100.0);
}


//Main function

int main()
{
    srand((unsigned int)time(NULL));
    
    double D;
    int N;
    int battleshipChoice;
    Battleship B;


    //Battlefield size

    do
    {
        printf("Enter battlefield size: ");

        scanf("%lf", &D);

        if (D <= 0)
        {
            printf("Invalid size. Enter a value > 0.\n");
        }

    } while (D <= 0);

    do
    {
        printf("Enter number of Escort Ships: ");
        scanf("%d", &N);

        if (N <= 0)
        {
            printf("Invalid number. Enter a value > 0.\n");
        }

    } while (N <= 0);

    do
    {
        printf("\nSelect Battleship Type:\n");
        printf("1. USS Iowa (U)\n");
        printf("2. MS King George V (M)\n");
        printf("3. Richelieu (R)\n");
        printf("4. Sovetsky Soyuz-class (S)\n");
        printf("Enter choice: ");
        scanf("%d",&battleshipChoice);
        
        if (battleshipChoice < 1 || battleshipChoice > 4)
        {
            printf("Invalid choice.\n");
        }

    } while (battleshipChoice < 1 || battleshipChoice > 4);
    
    switch (battleshipChoice)
    {
        case 1:
            strcpy(B.type, "U");
            break;

        case 2:
            strcpy(B.type, "M");
            break;

        case 3:
            strcpy(B.type, "R");
            break;

        default:
            strcpy(B.type, "S");
            break;
    }
    do
    {
        printf("\nEnter Battleship X position (0 - %.2f): ",D);
        scanf("%lf",&B.x);

        if (B.x < 0 || B.x > D)
        {
            printf("Invalid X position.\n");
        }

    } while (B.x < 0 || B.x > D);
    
    do
    {
        printf("Enter Battleship Y position (0 - %.2f): ",D);

        scanf("%lf",&B.y);

        if (B.y < 0 || B.y > D)
        {
            printf("Invalid Y position.\n");
        }

    } while (B.y < 0 ||B.y > D);
    
    B.initialX = B.x;
    B.initialY = B.y;
    
    do
    {
        printf("Enter Battleship maximum shell velocity (Vmax): ");
        scanf("%lf",&B.vMax);

        if (B.vMax <= 0)
        {
            printf("Invalid velocity.\n");
        }

    } while (B.vMax <= 0);

    EscortShip initialE[N];


    generateEscortShips(
        initialE,
        N,
        D,
        B.vMax
    );


    printf("\n========================================\n");

    printf("INITIAL ESCORT SHIPS\n");

    printf("========================================\n");


    for (int i = 0; i < N; i++)
    {
        printf("\nE%d\n",initialE[i].id);

        printf("  Type          : %s\n",initialE[i].type);

        printf("  Position      : (%.2f, %.2f)\n",initialE[i].x,initialE[i].y);

        printf("  Vmin          : %.2f m/s\n",initialE[i].vMin);

        printf("  Vmax          : %.2f m/s\n",initialE[i].vMax);

        printf("  Minimum Angle : %.2f degrees\n",initialE[i].angleMin);

        printf("  Maximum Angle : %.2f degrees\n",initialE[i].angleMax);

        printf("  Angle Range   : %.2f degrees\n",initialE[i].angleRange);

        printf("  Impact Power  : %.2f%%\n",initialE[i].impactPower * 100.0);

        printf("  Can Attack Once: YES\n");
    }

    int k;
    do
    {
        printf("\nEnter number of path points (k): ");

        scanf("%d",&k);
        if (k <= 0)
        {
            printf("k must be greater than 0.\n");
        }
    } while (k <= 0);
    
    Point path[k];
    
    generatePath(
        path,
        k,
        D
    );
    
    displayPath(
        path,
        k
    );
    
    saveInitialConditionsC(
        "part1_C_initial.txt",
        D,
        N,
        B,
        initialE,
        k,
        path
    );
    
    printf("\nInitial Part 1-C conditions saved to ");
    printf("part1_C_initial.txt\n");

    runFixedSimulationC(
        B,
        initialE,
        N
    );

    printf("\n\n");

    printf("########################################\n");

    printf("#          SIMULATION 1                #\n");

    printf("########################################\n");


    runPathSimulationC(
        1,
        B,
        initialE,
        N,
        path,
        k,
        -1,
        0.0
    );


    //Simulation 2 settings
    int t;

    double thetaMin;


    printf("\n\n========================================\n");

    printf("SIMULATION 2 SETTINGS\n");

    printf("========================================\n");


    if (k == 1)
    {
        t = 0;

        printf("Since k = 1, gun jam occurs before the only iteration.\n");
    }
    else
    {
        do
        {
            printf("Enter t (0 < t < k): ");

            scanf("%d",&t);

            if (t <= 0 || t >= k)
            {
                printf("t must satisfy 0 < t < k.\n");
            }

        } while (t <= 0 || t >= k);
    }

    do
    {
        printf("Enter theta_min (0 < theta_min < 30 degrees): ");

        scanf("%lf",&thetaMin);

        if (thetaMin <= 0 || thetaMin >= 30)
        {
            printf("theta_min must be between 0 and 30 degrees.\n");
        }

    } while (thetaMin <= 0 || thetaMin >= 30);


    //Simulation 2

    printf("\n\n");

    printf("########################################\n");

    printf("#          SIMULATION 2                #\n");

    printf("########################################\n");

    printf("Gun jams after iteration %d.\n",t);

    printf("After jam, vertical angle range is %.2f - 90 degrees.\n",thetaMin);


    runPathSimulationC(
        2,
        B,
        initialE,
        N,
        path,
        k,
        t,
        thetaMin
    );
    printf("\n\n");

    printf("========================================\n");

    printf("PART 1-C COMPLETED\n");

    printf("========================================\n");


    printf("\nGenerated files include:\n");

    printf("  part1_C_initial.txt\n");

    printf("  part1_C_fixed_final.txt\n");

    printf("  part1_C_sim1_iteration1.txt\n");

    printf("  part1_C_sim1_iteration2.txt\n");

    printf("  ...\n");

    printf("  part1_C_sim2_iteration1.txt\n");

    printf("  part1_C_sim2_iteration2.txt\n");

    printf("  ...\n");
    
    return 0;
}