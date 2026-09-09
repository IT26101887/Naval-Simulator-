#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define GRAVITY 9.81
#define TOLERANCE 1.0
#define MAX_IMPACT 1.0
#define MAX_ESCORTS 100
#define MAX_SIMULATION_TIME 10000.0

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

    double nextFireTime;
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

    double firingFrequency;
    double priorityScore;
} AttackTarget;


//Random number

double randomBetween(double min, double max)
{
    return min +
           ((double)rand() / RAND_MAX) *
           (max - min);
}


//Escort Ship Properties

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


//TE according to Escort Ship Type

double getEscortFireInterval(
        const char type[],
        double TE_EA,
        double TE_EB,
        double TE_EC,
        double TE_ED,
        double TE_EE)
{
    if (strcmp(type, "EA") == 0)
    {
        return TE_EA;
    }
    else if (strcmp(type, "EB") == 0)
    {
        return TE_EB;
    }
    else if (strcmp(type, "EC") == 0)
    {
        return TE_EC;
    }
    else if (strcmp(type, "ED") == 0)
    {
        return TE_ED;
    }
    else
    {
        return TE_EE;
    }
}


//Generate Escort Ships

void generateEscortShips(
        EscortShip E[],
        int N,
        double D,
        double battleshipVmax)
{
    char escortTypes[5][3] =
    {
        "EA",
        "EB",
        "EC",
        "ED",
        "EE"
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

        E[i].nextFireTime = 0.0;
    }
}

//Copy Escort Ships

void copyEscortShips(EscortShip destination[],EscortShip source[],int N)
{
    for (int i = 0; i < N; i++)
    {
        destination[i] = source[i];
    }
}


//Generate Battleship Path

void generatePath(Point path[],int k,double D)
{
    for (int i = 0; i < k; i++)
    {
        path[i].x = randomBetween(0.0, D);

        path[i].y = randomBetween(0.0, D);
    }
}


//Display Path

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

int calculateEscortShot(Battleship *B,EscortShip *E,double *flightTime)
{
    double firingVelocity;
    double firingAngle;
    double angleRadians;

    double dx;
    double dy;

    double projectileY;

    firingVelocity = randomBetween(E->vMin,E->vMax);

    firingAngle = randomBetween(E->angleMin,E->angleMax);

    angleRadians = firingAngle * M_PI / 180.0;

    dx = fabs(B->x - E->x);

    dy = B->y - E->y;

    printf("\nE%d attacks Battleship\n",E->id);

    printf("  Firing Velocity : %.2f m/s\n",firingVelocity);

    printf("  Firing Angle    : %.2f degrees\n",firingAngle);

    if (firingVelocity <= 0.0)
    {
        printf("  Result          : MISS\n");
        return 0;
    }

    //Same vertical line

    if (dx < 0.000001)
    {
        double maximumHeight =
            (
                firingVelocity *
                firingVelocity *
                sin(angleRadians) *
                sin(angleRadians)
            )
            /
            (2.0 * GRAVITY);

        if (dy >= 0.0 && dy <= maximumHeight && sin(angleRadians) > 0.0)
        {
            *flightTime =
                dy /
                (
                    firingVelocity *
                    sin(angleRadians)
                );

            printf("  Result          : HIT\n");

            printf("  Time to Hit     : %.2f seconds\n",*flightTime);

            return 1;
        }

        printf("  Result          : MISS\n");

        return 0;
    }

    if (cos(angleRadians) <= 0.0)
    {
        printf("  Result          : MISS\n");

        return 0;
    }

    *flightTime =
        dx /
        (
            firingVelocity *
            cos(angleRadians)
        );

    projectileY = E->y + firingVelocity * sin(angleRadians) * (*flightTime) -
                   0.5 * GRAVITY * (*flightTime) * (*flightTime);

    printf("  Projectile Y    : %.2f m\n",projectileY);

    printf("  Battleship Y    : %.2f m\n",B->y);

    if (fabs(projectileY - B->y) <= TOLERANCE)
    {
        printf("  Result          : HIT\n");

        printf("  Time to Hit     : %.2f seconds\n",*flightTime);

        return 1;
    }

    printf("  Result          : MISS\n");

    return 0;
}


//Escort Ships continuously attack Battleship

int escortAttackBattleshipB(Battleship *B,EscortShip E[],int N,double currentTime,
        
        double *cumulativeImpact,
        int *sinkingID,
        double *sinkingTime,
        double TE_EA,
        double TE_EB,
        double TE_EC,
        double TE_ED,
        double TE_EE)
{
    int battleshipSunk = 0;

    for (int i = 0; i < N; i++)
    {
        if (E[i].alive == 0)
        {
            continue;
        }

        if (currentTime + 0.000001 < E[i].nextFireTime)
        {
            continue;
        }

        double flightTime = 0.0;

        int hit = calculateEscortShot(B,&E[i],&flightTime);

        double TE = getEscortFireInterval(E[i].type,TE_EA,
                
                TE_EB,
                TE_EC,
                TE_ED,
                TE_EE
            );

        E[i].nextFireTime = currentTime + TE;

        printf("  TE for %s      : %.2f seconds\n",E[i].type,TE);

        if (hit)
        {
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

                *sinkingTime = currentTime + flightTime;

                battleshipSunk = 1;

                return battleshipSunk;
            }
        }
    }

    return battleshipSunk;
}


//Calculate B attack information

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

    //Same vertical line 

    if (dx < 0.000001)
    {
        if (90.0 < gunMinAngle)
        {
            return 0;
        }

        if (dy >= 0.0 && dy <= (firingVelocity * firingVelocity)/(2.0 * GRAVITY))
        {
            *chosenAngle = M_PI / 2.0;

            *flightTime = dy / firingVelocity;

            return 1;
        }

        return 0;
    }

    //rojectile equation

    discriminant = 
        pow(firingVelocity, 4.0)
        -
        GRAVITY *
        (
            GRAVITY * dx * dx
            +
            2.0 *
            dy *
            firingVelocity *
            firingVelocity
        );

    if (discriminant < 0.0)
    {
        return 0;
    }

    lowerAngle = atan((firingVelocity * firingVelocity - sqrt(discriminant))/(GRAVITY * dx));

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

    if (lowerAngle >= lowerLimit && lowerAngle <= M_PI / 2.0)
    {
        *chosenAngle = lowerAngle;

        *flightTime = dx /(firingVelocity * cos(lowerAngle));

        return 1;
    }

    if (upperAngle >= lowerLimit && upperAngle <= M_PI / 2.0)
    {
        *chosenAngle = upperAngle;

        *flightTime = dx /(firingVelocity * cos(upperAngle));

        return 1;
    }

    return 0;
}


//Calculate maximum impact among reachable Escort Ships

double findMaximumImpact(
        EscortShip E[],
        int N,
        Battleship *B,
        double gunMinAngle)
{
    double maximumImpact = 0.0;

    for (int i = 0; i < N; i++)
    {
        if (E[i].alive == 0)
        {
            continue;
        }

        double angle;
        double flightTime;

        if (calculateAttackData(
                B,
                &E[i],
                gunMinAngle,
                &angle,
                &flightTime))
        {
            if (E[i].impactPower >
                maximumImpact)
            {
                maximumImpact =
                    E[i].impactPower;
            }
        }
    }

    return maximumImpact;
}


//Find minimum and maximum flight times

void findFlightTimeRange(
        EscortShip E[],
        int N,
        Battleship *B,
        double gunMinAngle,
        double *minimumTime,
        double *maximumTime)
{
    *minimumTime = 1e9;
    *maximumTime = 0.0;

    for (int i = 0; i < N; i++)
    {
        if (E[i].alive == 0)
        {
            continue;
        }

        double angle;
        double flightTime;

        if (calculateAttackData(B,&E[i],gunMinAngle,&angle,&flightTime))
        {
            if (flightTime < *minimumTime)
            {
                *minimumTime = flightTime;
            }

            if (flightTime > *maximumTime)
            {
                *maximumTime = flightTime;
            }
        }
    }

    if (*minimumTime == 1e9)
    {
        *minimumTime = 0.0;
    }
}

//Calculate Priority score

double calculatePriorityScoreB(
        EscortShip *E,
        double flightTime,
        double minimumTime,
        double maximumTime,
        double maximumImpact,
        double minimumTE,
        double maximumTE,
        double TE)
{
    double impactScore;
    double frequencyScore;
    double timeScore;

    double priorityScore;

    if (maximumImpact > 0.0)
    {
        impactScore = E->impactPower / maximumImpact;
    }
    else
    {
        impactScore = 0.0;
    }

    if (maximumTE > minimumTE)
    {
        frequencyScore = (maximumTE - TE)/(maximumTE - minimumTE);
    }
    else
    {
        frequencyScore = 1.0;
    }
    
    if (maximumTime > minimumTime)
    {
        timeScore = (maximumTime - flightTime)/(maximumTime -minimumTime);
    }
    else
    {
        timeScore = 1.0;
    }
    
    //Custom strategy
    priorityScore = (impactScore * 0.50) + (frequencyScore * 0.30) + (timeScore * 0.20);

    return priorityScore;
}

void sortAttackTargets(AttackTarget targets[],int count)
{
    for (int i = 0;i < count - 1;i++)
    {
        for (int j = 0;j < count - i - 1;j++)
        {
            if (targets[j].priorityScore < targets[j + 1].priorityScore)
            {
                AttackTarget temp = targets[j];

                targets[j] = targets[j + 1];

                targets[j + 1] = temp;
            }
        }
    }
}


//Battleship attacks Escort ships

int battleshipAttackEscortsB(
        Battleship *B,
        EscortShip E[],
        int N,
        double gunMinAngle,
        double currentTime,
        double TB,
        double *battleEndTime,
        int attackOrder[],
        int *attackOrderCount,
        double TE_EA,
        double TE_EB,
        double TE_EC,
        double TE_ED,
        double TE_EE)
{
    AttackTarget targets[MAX_ESCORTS];

    int targetCount = 0;
    int hitCount = 0;

    double maximumImpact;

    double minimumFlightTime;
    double maximumFlightTime;

    double minimumTE;
    double maximumTE;

    *battleEndTime = currentTime;

    *attackOrderCount = 0;

    minimumTE = TE_EA;

    maximumTE = TE_EA;

    if (TE_EB < minimumTE)
        minimumTE = TE_EB;

    if (TE_EC < minimumTE)
        minimumTE = TE_EC;

    if (TE_ED < minimumTE)
        minimumTE = TE_ED;

    if (TE_EE < minimumTE)
        minimumTE = TE_EE;

    if (TE_EB > maximumTE)
        maximumTE = TE_EB;

    if (TE_EC > maximumTE)
        maximumTE = TE_EC;

    if (TE_ED > maximumTE)
        maximumTE = TE_ED;

    if (TE_EE > maximumTE)
        maximumTE = TE_EE;

    maximumImpact = findMaximumImpact(E,N,B,gunMinAngle);

    findFlightTimeRange(E,N,B,gunMinAngle,&minimumFlightTime,&maximumFlightTime);

    //Find all reachable targets
    for (int i = 0; i < N; i++)
    {
        if (E[i].alive == 0)
        {
            continue;
        }

        double firingAngle;
        double flightTime;

        int reachable = calculateAttackData(B,&E[i],gunMinAngle,&firingAngle,&flightTime);

        if (reachable)
        {
            double TE = getEscortFireInterval(E[i].type,TE_EA,TE_EB,TE_EC,TE_ED,TE_EE);

            targets[targetCount].escortID = E[i].id;

            targets[targetCount].firingAngle = firingAngle;

            targets[targetCount].flightTime = flightTime;

            targets[targetCount].impactPower = E[i].impactPower;

            targets[targetCount].firingFrequency = 1.0 / TE;

            targets[targetCount].priorityScore = calculatePriorityScoreB(&E[i],flightTime,
                minimumFlightTime,maximumFlightTime,
                maximumImpact,minimumTE,maximumTE,TE);

            targetCount++;
        }
    }

    printf("\n========================================\n");
    printf("CUSTOM PART 2-B COMPUTER STRATEGY\n");
    printf("========================================\n");

    printf("Current B firing time: %.2f seconds\n",currentTime);

    printf("TBq = %.2f seconds\n",TB);

    if (targetCount == 0)
    {
        printf("\nNo Escort Ships are currently ");
        printf("within Battleship attack range.\n");

        return 0;
    }

    printf("\nE ships within B attack range:\n");

    for (int i = 0;i < targetCount;i++)
    {
        printf(
            "E%d : Impact %.2f%%, "
            "TE %.2f s, "
            "Flight %.2f s, "
            "Priority %.3f\n",

            targets[i].escortID,

            targets[i].impactPower * 100.0,

            1.0 /
            targets[i].firingFrequency,

            targets[i].flightTime,

            targets[i].priorityScore
        );
    }

    sortAttackTargets(targets,targetCount);

    printf("\n========================================\n");
    printf("B ATTACK ORDER\n");
    printf("========================================\n");

    for (int i = 0; i < targetCount;i++)
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

        double firingTime = currentTime + (i * TB);

        double finishTime = firingTime + targets[i].flightTime;

        printf("\nAttack %d\n",i + 1);

        printf("  Target          : E%d\n",id);

        printf("  Firing Time     : %.2f seconds\n",firingTime);

        printf("  Firing Velocity : %.2f m/s\n",B->vMax);

        printf("  Firing Angle    : %.2f degrees\n",targets[i].firingAngle *180.0 / M_PI);

        printf("  Flight Time     : %.2f seconds\n",targets[i].flightTime);

        printf("  Priority Score  : %.3f\n",targets[i].priorityScore);

        attackOrder[*attackOrderCount] = id;

        (*attackOrderCount)++;
        
        printf("  Impact Time     : %.2f seconds\n",finishTime);

        printf("  Target Position : " "(%.2f, %.2f)\n",E[index].x,E[index].y);

        printf("  Result          : HIT\n");

        E[index].alive = 0;

        hitCount++;

        if (finishTime > *battleEndTime)
        {
            *battleEndTime = finishTime;
        }
    }

    printf("\n----------------------------------------\n");
    printf("FINAL B ATTACK ORDER\n");
    printf("----------------------------------------\n");

    for (int i = 0; i < *attackOrderCount; i++)
    {
        printf("%d. E%d\n",i + 1,attackOrder[i]);
    }

    return hitCount;
}


//Checking if any Escort Ship is alive 
int anyEscortAlive(EscortShip E[],int N)
{
    for (int i = 0; i < N; i++)
    {
        if (E[i].alive)
        {
            return 1;
        }
    }

    return 0;
}

//Save Initial Conditions
void saveInitialConditions(const char *filename,double D,int N,Battleship B,EscortShip E[],
        int k,
        Point path[],
        double TB,
        double TE_EA,
        double TE_EB,
        double TE_EC,
        double TE_ED,
        double TE_EE)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error creating %s\n",filename);

        return;
    }

    fprintf(file,"========================================\n");

    fprintf(file,"ADVANCED NAVAL BATTLE SIMULATOR\n");

    fprintf(file,"PART 2-B - INITIAL CONDITIONS\n");

    fprintf(file,"========================================\n\n");

    fprintf(file,"BATTLEFIELD\n");

    fprintf(file,"Size: %.2f x %.2f\n",D,D);

    fprintf(file,"Number of Escort Ships: %d\n\n",N);

    fprintf(file,"BATTLESHIP\n");

    fprintf(file,"Type: %s\n",B.type);

    fprintf(file,"Initial Position: ""(%.2f, %.2f)\n",B.initialX,B.initialY);

    fprintf(file,"Vmax: %.2f m/s\n",B.vMax);

    fprintf(file,"TBq: %.2f seconds\n\n",TB);

    fprintf(file,"ESCORT SHIP FIRING INTERVALS\n\n");

    fprintf(file,"TE^EA: %.2f seconds\n",TE_EA);

    fprintf(file,"TE^EB: %.2f seconds\n",TE_EB);

    fprintf(file,"TE^EC: %.2f seconds\n",TE_EC);

    fprintf(file,"TE^ED: %.2f seconds\n",TE_ED);

    fprintf(file,"TE^EE: %.2f seconds\n\n",TE_EE);

    fprintf(file,"ESCORT SHIPS\n\n");

    for (int i = 0; i < N; i++)
    {
        fprintf(file,"E%d\n",E[i].id);

        fprintf(file,"Type: %s\n",E[i].type);

        fprintf(file,"Position: " "(%.2f, %.2f)\n",E[i].x,E[i].y);

        fprintf(file,"Vmin: %.2f m/s\n",E[i].vMin);

        fprintf(file,"Vmax: %.2f m/s\n",E[i].vMax);

        fprintf(file,"Minimum Angle: %.2f degrees\n",E[i].angleMin);

        fprintf(file,"Maximum Angle: %.2f degrees\n",E[i].angleMax);
        
        fprintf(file,"Angle Range: %.2f degrees\n",E[i].angleRange);

        fprintf(file,"Impact Power: %.2f%%\n",E[i].impactPower * 100.0);

        fprintf(file,"TE: %.2f seconds\n\n",
            getEscortFireInterval(E[i].type,TE_EA,TE_EB,TE_EC,TE_ED,TE_EE)
        );
    }

    if (k > 0)
    {
        fprintf(file,"BATTLESHIP PATH\n\n");

        fprintf(file,"Number of Path Points: %d\n\n",k);

        for (int i = 0; i < k; i++)
        {
            fprintf(file,"Point %d: ""(%.2f, %.2f)\n",i + 1,path[i].x,path[i].y);
        }
    }

    fprintf(file,"\nGravity: %.2f m/s^2\n",GRAVITY);

    fprintf(file,"Initial Cumulative Impact " "on B: 0.00%%\n");
    fclose(file);
}

//Save Result

void saveResult(const char *filename,int simulationNumber,int iteration,
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
        double TB,
        double TE_EA,
        double TE_EB,
        double TE_EC,
        double TE_ED,
        double TE_EE)
{
    FILE *file = fopen(filename, "w");

    if (file == NULL)
    {
        printf("Error creating %s\n",filename);

        return;
    }

    fprintf(file,"========================================\n");

    fprintf(file,"ADVANCED NAVAL BATTLE SIMULATOR\n");

    fprintf(file,"PART 2-B\n");

    fprintf(file,"SIMULATION %d - ITERATION %d\n",simulationNumber,iteration);

    fprintf(file,"========================================\n\n");

    fprintf(file,"BATTLESHIP POSITION\n");

    fprintf(file,"(%.2f, %.2f)\n\n",currentPoint.x,currentPoint.y);

    fprintf(file,"GUN STATUS\n");

    if (gunJammed)
    {
        fprintf(file,"JAMMED\n");

        fprintf(file,"Allowed Vertical Angle: " "%.2f - 90.00 degrees\n\n",thetaMin);
    }
    else
    {
        fprintf(file,"NORMAL\n");

        fprintf(file,"Allowed Vertical Angle: " "0.00 - 90.00 degrees\n\n");
    }

    fprintf(file,"B GUN FIRING INTERVAL\n");

    fprintf(file,"TBq: %.2f seconds\n\n",TB);

    fprintf(file,"ESCORT GUN FIRING INTERVALS\n");

    fprintf(file,"TE^EA: %.2f seconds\n",TE_EA);

    fprintf(file,"TE^EB: %.2f seconds\n",TE_EB);

    fprintf(file,"TE^EC: %.2f seconds\n",TE_EC);

    fprintf(file,"TE^ED: %.2f seconds\n",TE_ED);

    fprintf(file,"TE^EE: %.2f seconds\n\n",TE_EE);

    fprintf(file,"CUSTOM COMPUTER STRATEGY\n");

    fprintf(file,"Priority factors:\n");

    fprintf(file,"50%% Impact Power\n");

    fprintf(file,"30%% Firing Frequency\n");

    fprintf(file,"20%% Flight Time\n\n");

    fprintf(file,"B ATTACK ORDER\n");

    if (attackOrderCount == 0)
    {
        fprintf(file,"No Escort Ships within ""B attack range.\n");
    }
    else
    {
        for (int i = 0; i < attackOrderCount; i++)
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

    fprintf(file, "ESCORT SHIP CONDITIONS\n\n");

    for (int i = 0; i < N; i++)
    {
        fprintf(file,"E%d\n",E[i].id);

        fprintf(file,"Type: %s\n",E[i].type);

        fprintf(file,"Position: (%.2f, %.2f)\n",E[i].x,E[i].y);

        fprintf(file,"Status: %s\n",E[i].alive ?"ALIVE" :"DESTROYED");

        fprintf(file,"Vmin: %.2f m/s\n",E[i].vMin);

        fprintf(file,"Vmax: %.2f m/s\n",E[i].vMax);

        fprintf(file,"Minimum Angle: %.2f degrees\n",E[i].angleMin);

        fprintf(file,"Maximum Angle: %.2f degrees\n",E[i].angleMax);

        fprintf(file,"Impact Power: %.2f%%\n",E[i].impactPower * 100.0);

        fprintf(file,"TE: %.2f seconds\n\n",getEscortFireInterval(E[i].type,TE_EA,TE_EB,TE_EC,TE_ED,TE_EE));
    }
    fclose(file);
}


//Run Fixed Simulation

void runFixedSimulationB(Battleship BStart,EscortShip initialE[],int N,
        double TB,
        double TE_EA,
        double TE_EB,
        double TE_EC,
        double TE_ED,
        double TE_EE)
{
    Battleship B = BStart;

    EscortShip E[N];

    copyEscortShips(E,initialE,N);

    double cumulativeImpact = 0.0;

    double currentTime = 0.0;

    double nextBFireTime = 0.0;

    int sinkingID = -1;

    double sinkingTime = 0.0;

    double battleEndTime = 0.0;

    int attackOrder[MAX_ESCORTS];

    int attackOrderCount = 0;

    Point currentPoint;

    currentPoint.x = B.x;
    currentPoint.y = B.y;

    printf("\n\n");
    printf("################################################\n");

    printf("PART 2-B FIXED BATTLEFIELD SIMULATION\n");

    printf("################################################\n");

    int safetyCounter = 0;

    while (currentTime <= MAX_SIMULATION_TIME)
    {
        safetyCounter++;

        if (safetyCounter > 100000)
        {
            break;
        }

        int eventOccurred = 0;

        int battleshipSunk = escortAttackBattleshipB(
                &B,
                E,
                N,
                currentTime,
                &cumulativeImpact,
                &sinkingID,
                &sinkingTime,
                TE_EA,
                TE_EB,
                TE_EC,
                TE_ED,
                TE_EE
            );

        if (battleshipSunk)
        {
            printf("\n***************************************\n");

            printf("BATTLESHIP DESTROYED\n");

            printf("Escort Ship: E%d\n",sinkingID);

            printf("Time of Final Impact: %.2f seconds\n",sinkingTime);

            printf("Cumulative Impact: 100.00%%\n");

            printf("***************************************\n");

            saveResult("part2_B_fixed_final.txt",1,1,B,E,N,1,sinkingID,
                sinkingTime,
                0,
                sinkingTime,
                currentPoint,
                0,
                0.0,
                cumulativeImpact,
                attackOrder,
                attackOrderCount,
                TB,
                TE_EA,
                TE_EB,
                TE_EC,
                TE_ED,
                TE_EE
            );

            return;
        }

        if (currentTime + 0.000001 >= nextBFireTime)
        {
            double totalBTime = 0.0;

            int hitCount = battleshipAttackEscortsB( &B,E,N,0.0,currentTime,
                    TB,
                    &battleEndTime,
                    attackOrder,
                    &attackOrderCount,
                    TE_EA,
                    TE_EB,
                    TE_EC,
                    TE_ED,
                    TE_EE
                );

            (void)totalBTime;

            printf("\n");
            printf("B destroyed %d Escort Ship(s).\n",hitCount);

            eventOccurred = 1;

            nextBFireTime += TB;

            if (!anyEscortAlive(E, N))
            {
                battleEndTime = currentTime;

                printf("\nAll Escort Ships have been destroyed.\n");

                break;
            }
        }

        double nextEventTime = nextBFireTime;

        for (int i = 0; i < N; i++)
        {
            if (E[i].alive == 0)
            {
                continue;
            }

            if (E[i].nextFireTime < nextEventTime)
            {
                nextEventTime = E[i].nextFireTime;
            }
        }

        if (nextEventTime <= currentTime + 0.000001)
        {
            nextEventTime = currentTime + 0.000001;
        }

        currentTime = nextEventTime;

        if (!eventOccurred && !anyEscortAlive(E, N))
        {
            break;
        }
    }

    printf("\n========================================\n");
    printf("FIXED SIMULATION RESULT\n");
    printf("========================================\n");

    printf("Battleship survived.\n");

    int remaining = 0;

    for (int i = 0; i < N; i++)
    {
        if (E[i].alive)
        {
            remaining++;
        }
    }

    printf("Remaining Escort Ships: %d\n",remaining);

    printf("Cumulative Impact on B: %.2f%%\n",cumulativeImpact * 100.0);

    printf("Battle End Time: %.2f seconds\n",currentTime);

    saveResult("part2_B_fixed_final.txt",1,1,B,E,N,0,-1,0.0,0,currentTime,currentPoint,
        0,
        0.0,
        cumulativeImpact,
        attackOrder,
        attackOrderCount,
        TB,
        TE_EA,
        TE_EB,
        TE_EC,
        TE_ED,
        TE_EE
    );
}


//Run Path Simulation

void runPathSimulationB(int simulationNumber,Battleship BStart,EscortShip initialE[],
        int N,
        Point path[],
        int k,
        int jamIteration,
        double thetaMin,
        double TB,
        double TE_EA,
        double TE_EB,
        double TE_EC,
        double TE_ED,
        double TE_EE)
{
    Battleship B = BStart;

    EscortShip E[N];

    copyEscortShips(E,initialE,N);

    double cumulativeImpact = 0.0;

    double globalTime = 0.0;

    printf("\n\n");
    printf("################################################\n");

    printf("SIMULATION %d STARTED\n",simulationNumber);

    printf("################################################\n");

    for (int iteration = 1;iteration <= k;iteration++)
    {
        B.x = path[iteration - 1].x;

        B.y = path[iteration - 1].y;

        printf("\n\n");
        printf("========================================\n");

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

        double iterationEndTime = globalTime + TB;

        while (globalTime < iterationEndTime - 0.000001)
        {
            int sinkingID = -1;

            double sinkingTime = 0.0;

            int battleshipSunk = escortAttackBattleshipB( &B,E,N,globalTime,&cumulativeImpact,
                    &sinkingID,
                    &sinkingTime,
                    TE_EA,
                    TE_EB,
                    TE_EC,
                    TE_ED,
                    TE_EE
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

                sprintf(filename,"part2_B_sim%d_iteration%d.txt",simulationNumber,iteration);

                int attackOrder[MAX_ESCORTS];

                int attackOrderCount = 0;

                saveResult(filename,simulationNumber,iteration,B,E,N,1,sinkingID,
                    sinkingTime,
                    0,
                    sinkingTime,
                    path[iteration - 1],
                    gunJammed,
                    thetaMin,
                    cumulativeImpact,
                    attackOrder,
                    attackOrderCount,
                    TB,
                    TE_EA,
                    TE_EB,
                    TE_EC,
                    TE_ED,
                    TE_EE
                );

                return;
            }

            if (globalTime < iterationEndTime)
            {
                double battleEndTime = 0.0;

                int attackOrder[MAX_ESCORTS];

                int attackOrderCount = 0;

                int hitCount = battleshipAttackEscortsB(&B,E,N,gunMinimumAngle,globalTime,
                        TB,
                        &battleEndTime,
                        attackOrder,
                        &attackOrderCount,
                        TE_EA,
                        TE_EB,
                        TE_EC,
                        TE_ED,
                        TE_EE
                    );

                printf("\nB destroyed %d Escort Ship(s).\n",hitCount);
            }

            //next E firing event.

            double nextTime = iterationEndTime;

            for (int i = 0; i < N; i++)
            {
                if (E[i].alive == 0)
                {
                    continue;
                }

                if (E[i].nextFireTime > globalTime && E[i].nextFireTime < nextTime)
                {
                    nextTime = E[i].nextFireTime;
                }
            }

            if (nextTime >= iterationEndTime)
            {
                globalTime = iterationEndTime;

                break;
            }

            globalTime = nextTime;

            if (!anyEscortAlive(E, N))
            {
                globalTime = iterationEndTime;

                break;
            }
        }

        printf("\n----------------------------------------\n");

        printf("ITERATION %d RESULT\n",iteration);

        printf("----------------------------------------\n");

        printf("Battleship survived.\n");

        printf("Cumulative Impact on B: %.2f%%\n",cumulativeImpact * 100.0);

        printf("Simulation Time: %.2f seconds\n",globalTime);

        int remaining = 0;

        for (int i = 0; i < N; i++)
        {
            if (E[i].alive)
            {
                remaining++;
            }
        }

        printf("Remaining Escort Ships: %d\n",remaining);

        //attack order information
        double dummyBattleTime = 0.0;

        int attackOrder[MAX_ESCORTS];

        int attackOrderCount = 0;

        EscortShip strategyE[N];

        copyEscortShips(strategyE,E,N);

        int hitCount = battleshipAttackEscortsB(&B,strategyE,N,gunMinimumAngle,
                globalTime,
                TB,
                &dummyBattleTime,
                attackOrder,
                &attackOrderCount,
                TE_EA,
                TE_EB,
                TE_EC,
                TE_ED,
                TE_EE
            );

        (void)hitCount;

        char filename[100];

        sprintf(filename,"part2_B_sim%d_iteration%d.txt",simulationNumber,iteration);

        saveResult(filename,simulationNumber,iteration,B,
            E,
            N,
            0,
            -1,
            0.0,
            0,
            globalTime,
            path[iteration - 1],
            gunJammed,
            thetaMin,
            cumulativeImpact,
            attackOrder,
            attackOrderCount,
            TB,
            TE_EA,
            TE_EB,
            TE_EC,
            TE_ED,
            TE_EE
        );

        if (!anyEscortAlive(E, N))
        {
            printf("\nAll Escort Ships destroyed.\n");

            break;
        }
    }

    printf("\n========================================\n");

    printf("SIMULATION %d COMPLETED\n",simulationNumber);

    printf("========================================\n");

    printf("Final cumulative impact on B: %.2f%%\n",cumulativeImpact * 100.0);
}


//Main function
void startSimulation()
{
    srand((unsigned int)time(NULL));

    double D;

    int N;

    int battleshipChoice;

    Battleship B;

    double TB;

    int TBchoice;

    double TE_EA;
    double TE_EB;
    double TE_EC;
    double TE_ED;
    double TE_EE;

    int TEchoice;


    //Battlefield size
    do
    {
        printf("Enter battlefield size: ");

        scanf("%lf",&D);

        if (D <= 0)
        {
            printf("Invalid size. Enter a value > 0.\n");
        }

    } while (D <= 0);


    //Number of Escort Ships
    do
    {
        printf("Enter number of Escort Ships: ");

        scanf("%d",&N);

        if (N <= 0 || N > MAX_ESCORTS)
        {
            printf("Enter a number between 1 and %d.\n",MAX_ESCORTS);
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


    //Battleship X position
    do
    {
        printf("\nEnter Battleship X position (0 - %.2f): ",D);

        scanf("%lf",&B.x);

        if (B.x < 0 || B.x > D)
        {
            printf("Invalid X position.\n");
        }

    } while (B.x < 0 || B.x > D);


    //Battleship Y position

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


    //Battleship maximum velocity

    do
    {
        printf(
            "Enter Battleship maximum "
            "shell velocity (Vmax): "
        );

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
        TB = randomBetween(1.0,10.0);

        printf("Randomly generated TBq = %.2f seconds\n",TB);
    }


    //TE values
    printf("\n========================================\n");

    printf("ESCORT SHIP FIRING INTERVALS - TE^p\n");

    printf("========================================\n");

    printf("1. Enter TE values manually\n");

    printf("2. Generate TE values randomly\n");

    do
    {
        printf("Enter choice: ");

        scanf("%d",&TEchoice);

        if (TEchoice < 1 || TEchoice > 2)
        {
            printf("Invalid choice.\n");
        }

    } while (TEchoice < 1 || TEchoice > 2);


    if (TEchoice == 1)
    {
        do
        {
            printf("\nEnter TE^EA (seconds): ");

            scanf("%lf",&TE_EA);

            if (TE_EA <= 0)
            {
                printf("TE must be greater than 0.\n");
            }

        } while (TE_EA <= 0);


        do
        {
            printf("Enter TE^EB (seconds): ");

            scanf("%lf",&TE_EB);

            if (TE_EB <= 0)
            {
                printf("TE must be greater than 0.\n");
            }

        } while (TE_EB <= 0);


        do
        {
            printf("Enter TE^EC (seconds): ");

            scanf("%lf",&TE_EC);

            if (TE_EC <= 0)
            {
                printf("TE must be greater than 0.\n");
            }

        } while (TE_EC <= 0);


        do
        {
            printf("Enter TE^ED (seconds): ");

            scanf("%lf",&TE_ED);

            if (TE_ED <= 0)
            {
                printf("TE must be greater than 0.\n");
            }

        } while (TE_ED <= 0);


        do
        {
            printf("Enter TE^EE (seconds): ");

            scanf("%lf",&TE_EE);

            if (TE_EE <= 0)
            {
                printf("TE must be greater than 0.\n");
            }

        } while (TE_EE <= 0);
    }
    else
    {
        TE_EA = randomBetween(1.0,10.0);

        TE_EB = randomBetween(1.0,10.0);

        TE_EC = randomBetween(1.0,10.0);

        TE_ED = randomBetween(1.0,10.0);

        TE_EE = randomBetween(1.0,10.0);

        printf("\nRandomly generated TE values:\n");

        printf("TE^EA = %.2f seconds\n",TE_EA);

        printf("TE^EB = %.2f seconds\n",TE_EB);

        printf("TE^EC = %.2f seconds\n",TE_EC);

        printf("TE^ED = %.2f seconds\n",TE_ED);

        printf("TE^EE = %.2f seconds\n",TE_EE);
    }
    
    //Generate Escort Ships
    EscortShip initialE[N];

    generateEscortShips(initialE,N,D,B.vMax);


    printf("\n========================================\n");

    printf("INITIAL ESCORT SHIPS\n");

    printf("========================================\n");


    for (int i = 0;i < N;i++)
    {
        double TE = getEscortFireInterval(initialE[i].type,TE_EA,TE_EB,TE_EC,TE_ED,TE_EE);

        printf("\nE%d\n",initialE[i].id);

        printf("  Type          : %s\n",initialE[i].type);

        printf("  Position      : ""(%.2f, %.2f)\n",initialE[i].x,initialE[i].y);

        printf("  Vmin          : %.2f m/s\n",initialE[i].vMin);

        printf("  Vmax          : %.2f m/s\n",initialE[i].vMax);

        printf("  Minimum Angle : %.2f degrees\n",initialE[i].angleMin);

        printf("  Maximum Angle : %.2f degrees\n",initialE[i].angleMax);

        printf("  Angle Range   : %.2f degrees\n",initialE[i].angleRange);

        printf("  Impact Power  : %.2f%%\n",initialE[i].impactPower *100.0);

        printf("  TE            : %.2f seconds\n",TE);
        printf("  Can Fire Continuously: YES\n");
    }
    
    //Generate path

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


    //Save initial conditions

    saveInitialConditions("part2_B_initial.txt",D,N,B,initialE,
        k,
        path,
        TB,
        TE_EA,
        TE_EB,
        TE_EC,
        TE_ED,
        TE_EE
    );

    printf("\nInitial Part 2-B conditions saved to part2_B_initial.txt\n");


    //Fixed simulation

    //runFixedSimulationB(B,initialE,N,TB,TE_EA,TE_EB,TE_EC,TE_ED,TE_EE);


    //Simulation 1

    printf("\n\n");

    printf("########################################\n");

    printf("#          SIMULATION 1                #\n");

    printf("########################################\n");


    runPathSimulationB(
        1,
        B,
        initialE,
        N,
        path,
        k,
        -1,
        0.0,
        TB,
        TE_EA,
        TE_EB,
        TE_EC,
        TE_ED,
        TE_EE
    );


    /* Simulation 2 settings */

    int t;

    double thetaMin;
    
    printf("\n\n");

    printf("========================================\n");

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
    
    runPathSimulationB(2,B,initialE,N,path,
        k,
        t,
        thetaMin,
        TB,
        TE_EA,
        TE_EB,
        TE_EC,
        TE_ED,
        TE_EE
    );
    
    printf("\n\n");

    printf("========================================\n");

    printf("PART 2-B COMPLETED\n");

    printf("========================================\n");

    printf("\nGenerated files include:\n");

    printf("  part2_B_initial.txt\n");

    printf("  part2_B_fixed_final.txt\n");

    printf("  part2_B_sim1_iteration1.txt\n");

    printf("  part2_B_sim1_iteration2.txt\n");

    printf("  ...\n");

    printf("  part2_B_sim2_iteration1.txt\n");

    printf("  part2_B_sim2_iteration2.txt\n");

    printf("  ...\n");
    return;
}

void showInstructions()
{
    printf("\n========================================\n");
    printf("          SIMULATOR INSTRUCTIONS\n");
    printf("========================================\n");

    printf("\n1. PURPOSE\n");
    printf("The simulator models a Battleship and multiple Escort Ships\n");
    printf("engaging each other on a battlefield.\n");

    printf("\n2. BATTLEFIELD SETUP\n");
    printf("You will enter the battlefield size, number of Escort Ships,\n");
    printf("Battleship properties, firing intervals and path points.\n");

    printf("\n3. BATTLESHIP\n");
    printf("The Battleship moves through the generated path points and\n");
    printf("fires at reachable Escort Ships.\n");

    printf("\n4. ESCORT SHIPS\n");
    printf("Escort Ships are randomly generated with different types,\n");
    printf("velocities, firing angles and impact powers.\n");

    printf("\n5. TBq\n");
    printf("TBq is the time interval between consecutive Battleship shots.\n");

    printf("\n6. TE^p\n");
    printf("TE is the firing interval for each Escort Ship type.\n");
    printf("Escort Ships can fire continuously according to their TE value.\n");

    printf("\n7. SIMULATION 1\n");
    printf("The Battleship operates normally throughout the simulation.\n");

    printf("\n8. SIMULATION 2\n");
    printf("After the specified iteration, the Battleship gun is restricted\n");
    printf("to a vertical firing angle between theta_min and 90 degrees.\n");

    printf("\n9. BATTLE RESULT\n");
    printf("The battle ends when the Battleship is destroyed or when all\n");
    printf("reachable Escort Ships have been destroyed.\n");

    printf("\n10. OUTPUT FILES\n");
    printf("Initial conditions and simulation results are saved as text files.\n");

    printf("\n========================================\n");
    printf("Press ENTER to return to the Main Menu.\n");
    printf("========================================\n");

    getchar();
    getchar();
}

void showFileContents(const char filename[])
{
    FILE *file = fopen(filename, "r");

    if (file == NULL)
    {
        printf("\nFile not found: %s\n", filename);
        printf("Please run the simulation first.\n");
        return;
    }

    char line[256];

    printf("\n========================================\n");
    printf("FILE: %s\n", filename);
    printf("========================================\n");

    while (fgets(line, sizeof(line), file) != NULL)
    {
        printf("%s", line);
    }

    fclose(file);

    printf("\n========================================\n");
    printf("End of file.\n");
    printf("========================================\n");
}

void simulationStatistics()
{
    int choice;
    int simulationNumber;
    int iteration;
    char filename[100];

    do
    {
        printf("\n========================================\n");
        printf("        SIMULATION STATISTICS\n");
        printf("========================================\n");

        printf("1. Initial Conditions\n");
        printf("2. Fixed Simulation Result\n");
        printf("3. Simulation 1 Result\n");
        printf("4. Simulation 2 Result\n");
        printf("5. Return to Main Menu\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                showFileContents("part2_B_initial.txt");
                break;

            case 2:
                showFileContents("part2_B_fixed_final.txt");
                break;

            case 3:
                printf("\nEnter Simulation 1 iteration number: ");
                scanf("%d", &iteration);

                if (iteration <= 0)
                {
                    printf("Invalid iteration number.\n");
                    break;
                }

                sprintf(
                    filename,
                    "part2_B_sim1_iteration%d.txt",
                    iteration
                );

                showFileContents(filename);
                break;

            case 4:
                printf("\nEnter Simulation 2 iteration number: ");
                scanf("%d", &iteration);

                if (iteration <= 0)
                {
                    printf("Invalid iteration number.\n");
                    break;
                }

                sprintf(
                    filename,
                    "part2_B_sim2_iteration%d.txt",
                    iteration
                );

                showFileContents(filename);
                break;

            case 5:
                break;

            default:
                printf("\nInvalid choice. Please enter 1-5.\n");
        }

    } while (choice != 5);
}

int main()
{
    int choice;

    do
    {
        printf("\n========================================\n");
        printf("     ADVANCED NAVAL BATTLE SIMULATOR\n");
        printf("========================================\n");

        printf("1. Start Simulation\n");
        printf("2. View Instructions\n");
        printf("3. Simulation Statistics\n");
        printf("4. Exit\n");

        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                startSimulation();
                break;

            case 2:
                showInstructions();
                break;

            case 3:
                simulationStatistics();
                break;

            case 4:
            {
                char confirm;

                printf("\nAre you sure you want to exit? (Y/N): ");
                scanf(" %c", &confirm);

                if (confirm == 'Y' || confirm == 'y')
                {
                    printf("\nExiting simulator...\n");
                    choice = 4;
                }
                else
                {
                    printf("\nReturning to Main Menu...\n");
                    choice = 0;
                }

                break;
            }

            default:
                printf("\nInvalid choice. Please enter 1-4.\n");
        }

    } while (choice != 4);

    return 0;
}