#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define GRAVITY 9.81
#define TOLERANCE 1.0
#define MAX_IMPACT 1.0
#define MAX_ESCORTS 100

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

typedef struct
{
    int escortID;
    double firingAngle;
    double flightTime;
    double impactPower;
    double priorityScore;
} AttackTarget;


// Random number
double randomBetween(double min, double max)
{
    return min + ((double)rand() / RAND_MAX) * (max - min);
}


// Set Escort Ship properties
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


// Generate Escort Ships
void generateEscortShips(
        EscortShip E[],
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

        E[i].angleMin =
            randomBetween(0.0, 90.0 - E[i].angleRange);

        E[i].angleMax =
            E[i].angleMin + E[i].angleRange;

        E[i].alive = 1;
        E[i].hasFired = 0;
    }
}


// Copy Escort Ships
void copyEscortShips(
        EscortShip destination[],
        EscortShip source[],
        int N)
{
    for (int i = 0; i < N; i++)
    {
        destination[i] = source[i];
    }
}


// Generate Battleship Path
void generatePath(
        Point path[],
        int k,
        double D)
{
    for (int i = 0; i < k; i++)
    {
        path[i].x = randomBetween(0.0, D);
        path[i].y = randomBetween(0.0, D);
    }
}


// Display Path
void displayPath(
        Point path[],
        int k)
{
    printf("\n========================================\n");
    printf("BATTLESHIP PATH\n");
    printf("========================================\n");

    for (int i = 0; i < k; i++)
    {
        printf("Point %d : (%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
    }
}


//Escort ships attack Battleship

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

        firingVelocity = randomBetween(E[i].vMin, E[i].vMax);

        firingAngle = randomBetween(E[i].angleMin, E[i].angleMax);

        angleRadians = firingAngle * M_PI / 180.0;

        dx = fabs(B->x - E[i].x);

        dy = B->y - E[i].y;

        printf("\nE%d attacks Battleship\n", E[i].id);

        printf("  Firing Velocity : %.2f m/s\n",firingVelocity);

        printf("  Firing Angle    : %.2f degrees\n",firingAngle);

        E[i].hasFired = 1;

        if (firingVelocity <= 0.0)
        {
            printf("  Result          : MISS\n");
            continue;
        }

        // Same vertical position
        if (dx < 0.000001)
        {
            double maximumHeight =
                (firingVelocity *
                 firingVelocity *
                 sin(angleRadians) *
                 sin(angleRadians))
                /
                (2.0 * GRAVITY);

            if (dy >= 0.0 && dy <= maximumHeight)
            {
                flightTime =
                    dy /
                    (firingVelocity *
                     sin(angleRadians));

                printf("  Result          : HIT\n");

                printf("  Time to Hit     : %.2f seconds\n",flightTime);

                *cumulativeImpact += E[i].impactPower;

                if (*cumulativeImpact > MAX_IMPACT)
                {
                    *cumulativeImpact = MAX_IMPACT;
                }

                printf("  Impact Power    : %.2f%%\n",E[i].impactPower * 100.0);

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

            continue;
        }
        if (cos(angleRadians) <= 0.0)
        {
            printf("  Result          : MISS\n");
            continue;
        }

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

            printf("  Impact Power    : %.2f%%\n",E[i].impactPower * 100.0);

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

int calculateAttackData(
        Battleship *B,
        EscortShip *E,
        double gunMinAngle,
        double *chosenAngle,
        double *flightTime)
{
    double firingVelocity = B->vMax;

    double dx;
    double dy;

    double discriminant;

    double lowerAngle;
    double upperAngle;

    double lowerLimit;

    if (firingVelocity <= 0.0)
    {
        return 0;
    }

    dx = fabs(E->x - B->x);

    dy = E->y - B->y;

    lowerLimit = gunMinAngle * M_PI / 180.0;

    // Same vertical line
    if (dx < 0.000001)
    {
        if (90.0 < gunMinAngle)
        {
            return 0;
        }

        if (dy >= 0.0 &&
            dy <=
            (firingVelocity *
             firingVelocity)
            /
            (2.0 * GRAVITY))
        {
            *chosenAngle = M_PI / 2.0;

            *flightTime = dy / firingVelocity;

            return 1;
        }

        return 0;
    }

    // Projectile equation
    discriminant = 
        pow(firingVelocity, 4.0)
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
        return 0;
    }

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

    if (lowerAngle >= lowerLimit &&
        lowerAngle <= M_PI / 2.0)
    {
        *chosenAngle = lowerAngle;

        *flightTime =
            dx /
            (firingVelocity *
             cos(lowerAngle));

        return 1;
    }

    if (upperAngle >= lowerLimit && upperAngle <= M_PI / 2.0)
    {
        *chosenAngle = upperAngle;

        *flightTime =
            dx /
            (firingVelocity *
             cos(upperAngle));

        return 1;
    }

    return 0;
}


//Custom strategy

double calculatePriorityScore(
        EscortShip *E,
        double flightTime)

{

    double impactScore =
        E->impactPower * 100.0;

    double timeScore =
        100.0 / (1.0 + flightTime);

    double priority =
        (impactScore * 0.70)
        +
        (timeScore * 0.30);

    return priority;
}

void sortAttackTargets(
        AttackTarget targets[],
        int count)
{
    for (int i = 0; i < count - 1; i++)
    {
        for (int j = 0; j < count - i - 1; j++)
        {
            if (targets[j].priorityScore <
                targets[j + 1].priorityScore)
            {
                AttackTarget temp = targets[j];

                targets[j] = targets[j + 1];

                targets[j + 1] = temp;
            }
        }
    }
}


//BattleShip attacks EScort ships using custom strategy
int battleshipAttackEscortsStrategy(
        Battleship *B,
        EscortShip E[],
        int N,
        double gunMinAngle,
        double TB,
        double *battleEndTime,
        int attackOrder[],
        int *attackOrderCount,
        double *totalBTime)
{
    AttackTarget targets[MAX_ESCORTS];

    int targetCount = 0;

    int hitCount = 0;

    *battleEndTime = 0.0;

    *attackOrderCount = 0;

    //Finding all escrt ships that battle ship can attack
    for (int i = 0; i < N; i++)
    {
        if (E[i].alive == 0)
        {
            continue;
        }

        double firingAngle;
        double flightTime;

        int reachable =
            calculateAttackData(
                B,
                &E[i],
                gunMinAngle,
                &firingAngle,
                &flightTime
            );

        if (reachable)
        {
            targets[targetCount].escortID = E[i].id;

            targets[targetCount].firingAngle = firingAngle;

            targets[targetCount].flightTime = flightTime;

            targets[targetCount].impactPower = E[i].impactPower;

            targets[targetCount].priorityScore = calculatePriorityScore(&E[i],flightTime);
            targetCount++;
        }
    }

    //Sort according tocustom strategy

    sortAttackTargets(targets,targetCount);

    printf("\n========================================\n");
    printf("CUSTOM COMPUTER ATTACK STRATEGY\n");
    printf("========================================\n");

    printf("TB = %.2f seconds\n", TB);

    if (targetCount == 0)
    {
        printf("\nNo Escort Ships are currently within ");
        printf("Battleship attack range.\n");

        return 0;
    }

    printf("\nE ships within B attack range:\n");

    for (int i = 0; i < targetCount; i++)
    {
        printf("E%d : Impact %.2f%%, Flight %.2f s, "
               "Priority %.2f\n",
               targets[i].escortID,
               targets[i].impactPower * 100.0,
               targets[i].flightTime,
               targets[i].priorityScore);
    }

    //Attack in priority order.

    printf("\n========================================\n");
    printf("B ATTACK ORDER\n");
    printf("========================================\n");

    for (int i = 0; i < targetCount; i++)
    {
        int id = targets[i].escortID;

        int index = -1;

        for (int j = 0; j < N; j++)
        {
            if (E[j].id == id)
            {
                index = j;
                break;
            }
        }

        if (index == -1)
        {
            continue;
        }

        double firingStartTime = i * TB;

        double firingFinishTime = firingStartTime + targets[i].flightTime;

        printf("\nAttack %d\n", i + 1);

        printf("  Target          : E%d\n",id);

        printf("  Firing Time     : %.2f seconds\n",firingStartTime);

        printf("  Firing Velocity : %.2f m/s\n",B->vMax);

        printf("  Firing Angle    : %.2f degrees\n",targets[i].firingAngle * 180.0 / M_PI);

        printf("  Flight Time     : %.2f seconds\n",targets[i].flightTime);

        printf("  Priority Score  : %.2f\n",targets[i].priorityScore);

        attackOrder[*attackOrderCount] = id;

        (*attackOrderCount)++;

        double angle = targets[i].firingAngle;

        double dx = fabs(E[index].x - B->x);

        double projectileY;

        double actualFlightTime = targets[i].flightTime;

        if (dx < 0.000001)
        {
            projectileY =
                B->y +
                B->vMax *
                sin(angle) *
                actualFlightTime -
                0.5 *
                GRAVITY *
                actualFlightTime *
                actualFlightTime;
        }
        else
        {
            projectileY =
                B->y +
                B->vMax *
                sin(angle) *
                actualFlightTime -
                0.5 *
                GRAVITY *
                actualFlightTime *
                actualFlightTime;
        }

        printf("  Target Position : (%.2f, %.2f)\n",E[index].x,E[index].y);

        printf("  Projectile Y    : %.2f m\n",projectileY);

        if (fabs(projectileY - E[index].y) <= TOLERANCE)
        {
            printf("  Result          : HIT\n");

            E[index].alive = 0;

            hitCount++;
        }
        else
        {
            printf("  Result          : MISS\n");
        }

        if (firingFinishTime > *battleEndTime)
        {
            *battleEndTime = firingFinishTime;
        }
    }

    //Total B firing timeline.

    if (targetCount > 0)
    {
        *totalBTime = (targetCount - 1) * TB;
    }
    else
    {
        *totalBTime = 0.0;
    }

    printf("\n----------------------------------------\n");

    printf("FINAL B ATTACK ORDER\n");

    printf("----------------------------------------\n");

    for (int i = 0;i < *attackOrderCount;i++)
    {
        printf("%d. E%d\n", i + 1,attackOrder[i]);
    }

    printf("\nTotal B firing interval time: %.2f seconds\n",*totalBTime);

    return hitCount;
}


//saving initial conditions
void saveInitialConditions(
        const char *filename,
        double D,
        int N,
        Battleship B,
        EscortShip E[],
        int k,
        Point path[],
        double TB)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error creating %s\n",filename);

        return;
    }

    fprintf(file,"========================================\n");

    fprintf(file,"ADVANCED NAVAL BATTLE SIMULATOR\n");

    fprintf(file,"PART 2-A - INITIAL CONDITIONS\n");

    fprintf(file,"========================================\n\n");

    fprintf(file,"BATTLEFIELD\n");

    fprintf(file,"Size: %.2f x %.2f\n",D, D);

    fprintf(file,"Number of Escort Ships: %d\n\n",N);

    fprintf(file,"BATTLESHIP\n");

    fprintf(file,"Type: %s\n",B.type);

    fprintf(file,"Initial Position: (%.2f, %.2f)\n",B.initialX,B.initialY);

    fprintf(file,"Vmax: %.2f m/s\n",B.vMax);

    fprintf(file,"TBq - Time Between B Gun Firings: %.2f seconds\n\n",TB);

    fprintf(file,"ESCORT SHIPS\n\n");

    for (int i = 0; i < N; i++)
    {
        fprintf(file,"E%d\n",E[i].id);

        fprintf(file,"Type: %s\n",E[i].type);

        fprintf(file,"Position: (%.2f, %.2f)\n",E[i].x,E[i].y);

        fprintf(file,"Vmin: %.2f m/s\n",E[i].vMin);

        fprintf(file,"Vmax: %.2f m/s\n",E[i].vMax);

        fprintf(file,"Minimum Angle: %.2f degrees\n", E[i].angleMin);

        fprintf(file,"Maximum Angle: %.2f degrees\n",E[i].angleMax);

        fprintf(file,"Angle Range: %.2f degrees\n",E[i].angleRange);

        fprintf(file,"Impact Power: %.2f%%\n",E[i].impactPower * 100.0);

        fprintf(file, "\n");
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

// Save result
void saveResult(
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
        double cumulativeImpact,
        int attackOrder[],
        int attackOrderCount,
        double TB)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error creating %s\n",filename);

        return;
    }

    fprintf(file,"========================================\n");

    fprintf(file,"ADVANCED NAVAL BATTLE SIMULATOR\n");

    fprintf(file,"PART 2-A\n");

    fprintf(file,"SIMULATION %d - ITERATION %d\n",simulationNumber,iteration);

    fprintf(file,"========================================\n\n");

    fprintf(file,"BATTLESHIP POSITION\n");

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

    fprintf(file,"B GUN FIRING INTERVAL\n");

    fprintf(file,"TBq: %.2f seconds\n\n",TB);

    fprintf(file,"CUSTOM COMPUTER STRATEGY\n");

    fprintf(file,"Strategy: Prioritize higher-impact Escort Ships\n");

    fprintf(file,"and favour shorter flight times.\n\n");

    fprintf(file,"B ATTACK ORDER\n");

    if (attackOrderCount == 0)
    {
        fprintf(file,"No Escort Ships within B attack range.\n");
    }
    else
    {
        for (int i = 0; i < attackOrderCount;i++)
        {
            fprintf(file,"%d. E%d\n",i + 1,attackOrder[i]);
        }
    }

    fprintf(file, "\n");

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

    fprintf(file,"Vmax: %.2f m/s\n",B.vMax);

    fprintf(file,"Cumulative Damage: %.2f%%\n\n",cumulativeImpact * 100.0);

    fprintf(file,"ESCORT SHIP CONDITIONS\n\n");

    for (int i = 0; i < N; i++)
    {
        fprintf(file,"E%d\n",E[i].id);

        fprintf(file,"Type: %s\n",E[i].type);

        fprintf(file,"Position: (%.2f, %.2f)\n",E[i].x,E[i].y);

        fprintf(file,"Status: %s\n",E[i].alive ?"ALIVE" :"DESTROYED");

        fprintf(file,"Has Fired: %s\n",E[i].hasFired ?"YES" :"NO");

        fprintf(file,"Vmin: %.2f m/s\n",E[i].vMin);

        fprintf(file,"Vmax: %.2f m/s\n",E[i].vMax);

        fprintf(file,"Minimum Angle: %.2f degrees\n",E[i].angleMin);

        fprintf(file,"Maximum Angle: %.2f degrees\n",E[i].angleMax);

        fprintf(file,"Impact Power: %.2f%%\n\n",E[i].impactPower * 100.0);
    }
    fclose(file);
}

void runFixedSimulation(
        Battleship BStart,
        EscortShip initialE[],
        int N,
        double TB)
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
    printf("PART 2-A FIXED BATTLEFIELD SIMULATION\n");
    printf("################################################\n");

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

        int attackOrder[MAX_ESCORTS];

        int attackOrderCount = 0;

        saveResult(
            "part2_A_fixed_final.txt",
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
            cumulativeImpact,
            attackOrder,
            attackOrderCount,
            TB
        );

        return;
    }

    double battleEndTime = 0.0;

    int attackOrder[MAX_ESCORTS];

    int attackOrderCount = 0;

    double totalBTime = 0.0;

    int hitCount =
        battleshipAttackEscortsStrategy(
            &B,
            E,
            N,
            0.0,
            TB,
            &battleEndTime,
            attackOrder,
            &attackOrderCount,
            &totalBTime
        );

    printf("\n========================================\n");
    printf("FIXED SIMULATION RESULT\n");
    printf("========================================\n");

    printf("Battleship survived.\n");

    printf("Escort Ships destroyed by B: %d\n",hitCount);

    printf("Cumulative Impact on B: %.2f%%\n",cumulativeImpact * 100.0);

    saveResult(
        "part2_A_fixed_final.txt",
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
        cumulativeImpact,
        attackOrder,
        attackOrderCount,
        TB
    );
}

//path simulation
void runPathSimulation(
        int simulationNumber,
        Battleship BStart,
        EscortShip initialE[],
        int N,
        Point path[],
        int k,
        int jamIteration,
        double thetaMin,
        double TB)
{
    Battleship B = BStart;

    EscortShip E[N];

    copyEscortShips(E,initialE,N);

    double cumulativeImpact = 0.0;

    printf("\n\n");
    printf("################################################\n");
    printf("SIMULATION %d STARTED\n",simulationNumber);
    printf("################################################\n");

    for (int iteration = 1;iteration <= k;iteration++)
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

        printf("TBq: %.2f seconds\n",TB);

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

            sprintf(
                filename,
                "part2_A_sim%d_iteration%d.txt",
                simulationNumber,
                iteration
            );
            int attackOrder[MAX_ESCORTS];
            int attackOrderCount = 0;

            saveResult(
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
                cumulativeImpact,
                attackOrder,
                attackOrderCount,
                TB
            );

            break;
        }

        double battleEndTime = 0.0;

        int attackOrder[MAX_ESCORTS];

        int attackOrderCount = 0;

        double totalBTime = 0.0;

        int hitCount =
            battleshipAttackEscortsStrategy(
                &B,
                E,
                N,
                gunMinimumAngle,
                TB,
                &battleEndTime,
                attackOrder,
                &attackOrderCount,
                &totalBTime
            );

        printf("\n----------------------------------------\n");

        printf("ITERATION %d RESULT\n",iteration);

        printf("----------------------------------------\n");

        printf("Battleship survived.\n");

        printf("Escort Ships destroyed by B: %d\n",hitCount);

        printf("Cumulative Impact on B: %.2f%%\n",cumulativeImpact * 100.0);

        printf("Battle End Time: %.2f seconds\n",battleEndTime);

        char filename[100];

        sprintf(filename,"part2_A_sim%d_iteration%d.txt",simulationNumber,iteration);

        saveResult(
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
            cumulativeImpact,
            attackOrder,
            attackOrderCount,
            TB
        );
    }

    printf("\n========================================\n");

    printf("SIMULATION %d COMPLETED\n",simulationNumber);

    printf("========================================\n");

    printf("Final cumulative impact on B: %.2f%%\n",cumulativeImpact * 100.0);
}


//main function
int main()
{
    srand((unsigned int)time(NULL));

    double D;

    int N;

    int battleshipChoice;

    Battleship B;

    double TB;

    int TBchoice;

    //battlefield size
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

        if (N <= 0 || N > MAX_ESCORTS)
        {
            printf("Enter a number between 1 and %d.\n",
                   MAX_ESCORTS);
        }

    } while (N <= 0 || N > MAX_ESCORTS);


    //Battleship Type
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

    } while (B.y < 0 || B.y > D);


    B.initialX = B.x;
    B.initialY = B.y;

    //Battleship maximum shell velocity
    do
    {
        printf("Enter Battleship maximum shell velocity (Vmax): ");

        scanf("%lf",&B.vMax);

        if (B.vMax <= 0)
        {
            printf("Invalid velocity.\n");
        }

    } while (B.vMax <= 0);


    //TBq
    printf("\n========================================\n");

    printf("B GUN FIRING INTERVAL - TBq\n");

    printf("========================================\n");

    printf("1. Enter TBq manually\n");

    printf("2. Generate TBq randomly\n");

    do
    {
        printf("Enter choice: ");

        scanf("%d",&TBchoice);

        if (TBchoice < 1 || TBchoice > 2)
        {
            printf("Invalid choice.\n");
        }

    } while (TBchoice < 1 || TBchoice > 2);


    if (TBchoice == 1)
    {
        do
        {
            printf("Enter TBq (seconds): ");

            scanf("%lf",&TB);

            if (TB <= 0)
            {
                printf("TBq must be greater than 0.\n");
            }

        } while (TB <= 0);
    }
    else
    {
        TB =randomBetween(1.0, 10.0);
        printf("Randomly generated TBq = %.2f seconds\n",TB);
    }
    
    // Generate Escort Ships
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

    for (int i = 0;i < N;i++)
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


    //Path points
    
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

    generatePath(path,k,D);

    displayPath(path,k);


    saveInitialConditions(
        "part2_A_initial.txt",
        D,
        N,
        B,
        initialE,
        k,
        path,
        TB
    );

    printf("\nInitial Part 2-A conditions saved to ");
    printf("part2_A_initial.txt\n");
    
    // Fixed simulation
    runFixedSimulation(
        B,
        initialE,
        N,
        TB
    );
    
    // Simulation 1
    printf("\n\n");

    printf("########################################\n");

    printf("#          SIMULATION 1                #\n");

    printf("########################################\n");

    runPathSimulation(
        1,
        B,
        initialE,
        N,
        path,
        k,
        -1,
        0.0,
        TB
    );

    //simulation 2 settings
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

            if (t <= 0 ||t >= k)
            {
                printf("t must satisfy 0 < t < k.\n");
            }

        } while (t <= 0 ||t >= k);
    }
    
    do
    {
        printf("Enter theta_min (0 < theta_min < 30 degrees): ");

        scanf("%lf",&thetaMin);

        if (thetaMin <= 0 ||thetaMin >= 30)
        {
            printf("theta_min must be between 0 and 30 degrees.\n");
        }

    } while (thetaMin <= 0 ||thetaMin >= 30);

    printf("\n\n");

    printf("########################################\n");

    printf("#          SIMULATION 2                #\n");

    printf("########################################\n");

    printf("Gun jams after iteration %d.\n",t);

    printf("After jam, vertical angle range is %.2f - 90 degrees.\n",thetaMin);
    
    runPathSimulation(2,B,initialE,N,path,k,t,thetaMin,TB);
    
    printf("\n\n");

    printf("========================================\n");

    printf("PART 2-A COMPLETED\n");

    printf("========================================\n");

    printf("\nGenerated files include:\n");

    printf("  part2_A_initial.txt\n");

    printf("  part2_A_fixed_final.txt\n");

    printf("  part2_A_sim1_iteration1.txt\n");

    printf("  part2_A_sim1_iteration2.txt\n");

    printf("  ...\n");

    printf("  part2_A_sim2_iteration1.txt\n");

    printf("  part2_A_sim2_iteration2.txt\n");

    printf("  ...\n");

    return 0;
}