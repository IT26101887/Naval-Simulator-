#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>


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
    double impactPower;
    double angleRange;
}EscortShip;

typedef struct
{
    char type[3];
    double x;
    double y;
    double vMax;
}Battleship;

int main()
{
    srand(time(NULL));
    double D;
    int N;
    double firingVelocity;
    double firingAngle;
    int battleshipChoice;
    double distance;
    double projectileRange;
    double MaxprojectileRange;
    double dx;
    double dy;
    double discriminant;
    double angleRadians;
    double lowerAngle;
    double upperAngle;
    double flightTime;
    double projectileY;
    double tolerance;
    int hitCount = 0;
    double eFiringVelocity;
    double eFiringAngle;
    double eAngleRadians;
    double eDx;
    double eDy;
    double eFlightTime;
    double eProjectileY;

    int battleshipSunk = 0;
    int sinkingEscortIndex = -1;
    double battleEndTime = 0.0;

    Battleship B;

    //BattleField Size
    do                                               
    {
        printf("Enter the battlefield size : ");
        scanf("%lf",&D);

        if(D<=0)
        {
            printf("Invalid Size entered. %.2f must be greater than 0\n",D);
        }
    }while(D<=0);

    //Number of Escort Ships
    do
    {
        printf("Enter the number of Escort Ships : ");
        scanf("%d",&N);

        if(N<=0)
        {
            printf("Invalid Number of Escort Ships\n");
        }
        
    } while (N<=0);

    int escortHit[N];
    double escortHitTime[N];
    double escortHitAngle[N];
    double escortHitVelocity[N];
    
    printf("Battlefield Size : %.2f x %.2f\n",D,D);
    printf("Number of Escort Ships : %d\n",N);


    //Select the Battle Ship Type
    do
    {
        printf("\nSelect BattleShip Type : \n");
        printf("1. USS Iowa (U) \n");
        printf("2. MS King George V (M) \n");
        printf("3. Richelieu (R) \n");
        printf("4. Sovetsky Soyuz-class (S)\n");
        printf("Enter your choice of BattleShip : ");
        scanf("%d",&battleshipChoice);

        if(battleshipChoice<1 || battleshipChoice>4)
        {
            printf("Invalid Choice. Select between 1 - 4\n");
        }
        
    } while (battleshipChoice<1 || battleshipChoice>4);

    switch(battleshipChoice)
    {
        case 1:
         strcpy(B.type,"U");
         break;

        case 2:
         strcpy(B.type, "M");
         break;
    
        case 3:
         strcpy(B.type, "R");
         break;
    
        case 4:
         strcpy(B.type, "S");
         break;
    }
    
    //Position of the BattleShip
    do
    {
        printf("\nEnter Battleship X position (0 - %.2f): ", D);
        scanf("%lf", &B.x);

        if (B.x < 0 || B.x > D)
        {
            printf("Invalid X position. It must be between 0 and %.2f.\n", D);
        }

    } while (B.x < 0 || B.x > D);

    do
    {
        printf("Enter Battleship Y position (0 - %.2f): ", D);
        scanf("%lf", &B.y);

        if (B.y < 0 || B.y > D)
        {
            printf("Invalid Y position. It must be between 0 and %.2f.\n", D);
        }

    } while (B.y < 0 || B.y > D);

    //BattleShip Maximum Velocity
    do
    {
        printf("Enter Battleship maximum shell velocity (Vmax): ");
        scanf("%lf", &B.vMax);

        if (B.vMax <= 0)
        {
            printf("Invalid velocity. Vmax must be greater than 0.\n");
        }

    } while (B.vMax <= 0);

    EscortShip E[N];
    char escortTypes[5][3] = {"EA", "EB", "EC", "ED", "EE"};

    for (int i = 0; i < N; i++)
    {
        E[i].id = i + 1;
        E[i].x = ((double)rand() / RAND_MAX) * D;
        E[i].y = ((double)rand() / RAND_MAX) * D;
        
        int typeIndex = rand() % 5;
        strcpy(E[i].type, escortTypes[typeIndex]);
    }

    // Setting Properties according to escort ship type
    for (int i = 0; i < N; i++)
    {
        if (strcmp(E[i].type, "EA") == 0)
        {
            E[i].impactPower = 0.08;
            E[i].angleRange = 20;
        }
        else if (strcmp(E[i].type, "EB") == 0)
        {
            E[i].impactPower = 0.06;
            E[i].angleRange = 30;
        }
        else if (strcmp(E[i].type, "EC") == 0)
        {
            E[i].impactPower = 0.07;
            E[i].angleRange = 25;
        }
        else if (strcmp(E[i].type, "ED") == 0)
        {
            E[i].impactPower = 0.05;
            E[i].angleRange = 50;
        }
        else if (strcmp(E[i].type, "EE") == 0)
        {
            E[i].impactPower = 0.04;
            E[i].angleRange = 70;
        }
    }

    //Generate Random velocities
    for (int i = 0; i < N; i++)
    {
        if (strcmp(E[i].type, "EA") == 0)
        {
            E[i].vMax = 1.2 * B.vMax;

            E[i].vMin = ((double)rand() / RAND_MAX) * E[i].vMax;
        }
        else
        {
            E[i].vMax = ((double)rand() / RAND_MAX) * B.vMax;

            E[i].vMin = ((double)rand() / RAND_MAX) * E[i].vMax;
        }
    }

    // Generate random minimum angle and calculate maximum angle
    for (int i = 0; i < N; i++)
    {
        E[i].angleMin = ((double)rand() / RAND_MAX) *
                    (90 - E[i].angleRange);
        
        E[i].angleMax = E[i].angleMin + E[i].angleRange;
    }
    
    //Displaying generated Escort Ship properties
    
    printf("\nEscort Ships:\n");

    for (int i = 0; i < N; i++)
    {
        printf("\nE%d\n", E[i].id);
        printf("  Type          : %s\n", E[i].type);
        printf("  Position      : (%.2f, %.2f)\n", E[i].x, E[i].y);
        printf("  Vmin          : %.2f\n", E[i].vMin);
        printf("  Vmax          : %.2f\n", E[i].vMax);
        
        printf("  Angle Range   : %.2f degrees\n", E[i].angleRange);
        printf("  Minimum Angle : %.2f degrees\n", E[i].angleMin);
        printf("  Maximum Angle : %.2f degrees\n", E[i].angleMax);
        printf("  Impact Power  : %.2f\n", E[i].impactPower);
    }
    
    // Generate Battleship firing velocity
    firingVelocity = ((double)rand() / RAND_MAX) * B.vMax;

    // Generate Battleship firing angle
    firingAngle = ((double)rand() / RAND_MAX) * 90;

    // Display Battleship firing setup
    printf("\nBattleship Firing Setup:\n");
    printf("  Firing Velocity : %.2f\n", firingVelocity);
    printf("  Firing Angle    : %.2f degrees\n", firingAngle);  

    printf("\nDistance from Battleship to Escort Ships:\n");

    //Distance between the BattleShip and Escort Ships
    for(int i = 0; i < N; i++)
    {
    distance = sqrt(
        (E[i].x - B.x) * (E[i].x - B.x) +
        (E[i].y - B.y) * (E[i].y - B.y)
    );

    printf("Distance to E%d: %.2f\n", E[i].id, distance);
    }

    // Calculate Battleship projectile range
    projectileRange = (firingVelocity * firingVelocity *
                        sin(2 * firingAngle * M_PI / 180.0)) / 9.81;

    MaxprojectileRange = (B.vMax * B.vMax) / 9.81;

    printf("\nBattleship Projectile Range: %.2f\n", projectileRange);
    printf("\nEscort Ships Within Firing Range:\n");
    printf("Battleship Maximum Possible Range: %.2f m\n",MaxprojectileRange);

    for (int i = 0; i < N; i++)
    {
        distance = sqrt(
        (E[i].x - B.x) * (E[i].x - B.x) +
        (E[i].y - B.y) * (E[i].y - B.y)
        );

        if (distance <= MaxprojectileRange)
        {
            printf("E%d can be reached by the Battleship.\n", E[i].id);
        }
        else
        {
            printf("E%d cannot be reached by the Battleship.\n", E[i].id);
        }
    }

    // Save initial battlefield conditions to text file

    FILE *initialFile;

    initialFile = fopen("part1_A_initial.txt", "w");

    if (initialFile == NULL)
    {
        printf("\nError: Could not create initial conditions file.\n");
    }
    else
    {
        fprintf(initialFile, "========================================\n");
        fprintf(initialFile, "ADVANCED NAVAL BATTLE SIMULATOR\n");
        fprintf(initialFile, "PART 1-A - INITIAL CONDITIONS\n");
        fprintf(initialFile, "========================================\n\n");

        // Battlefield information
        fprintf(initialFile, "BATTLEFIELD\n");
        fprintf(initialFile, "Battlefield Size: %.2f x %.2f\n",
                    D, D);
        fprintf(initialFile, "Number of Escort Ships: %d\n\n",
            N);

        // Battleship information
        fprintf(initialFile, "BATTLESHIP\n");
        fprintf(initialFile, "Type: %s\n", B.type);
        fprintf(initialFile, "Initial Position: (%.2f, %.2f)\n",
                       B.x, B.y);
        fprintf(initialFile, "Vmax: %.2f m/s\n\n",
                     B.vMax);

        // Escort ship information
        fprintf(initialFile, "ESCORT SHIPS\n");

        for (int i = 0; i < N; i++)
        {
            fprintf(initialFile, "\nE%d\n", E[i].id);

            fprintf(initialFile, "Type: %s\n",
                    E[i].type);

            fprintf(initialFile, "Position: (%.2f, %.2f)\n",
                          E[i].x, E[i].y);

            fprintf(initialFile, "Vmin: %.2f m/s\n",
                          E[i].vMin);

            fprintf(initialFile, "Vmax: %.2f m/s\n",
                          E[i].vMax);

            fprintf(initialFile, "Minimum Angle: %.2f degrees\n",
                                E[i].angleMin);

            fprintf(initialFile, "Maximum Angle: %.2f degrees\n",
                           E[i].angleMax);

            fprintf(initialFile, "Angle Range: %.2f degrees\n",
                              E[i].angleRange);

            fprintf(initialFile, "Impact Power: %.2f\n",
                               E[i].impactPower);
        }

        // Other implementation values
        fprintf(initialFile, "\nOTHER IMPLEMENTATION VALUES\n");
        fprintf(initialFile,"  Firing Velocity : %.2f\n", firingVelocity);
        fprintf(initialFile,"  Firing Angle    : %.2f degrees\n", firingAngle);
        fprintf(initialFile, "Gravity: 9.81 m/s^2\n");

        fclose(initialFile);

        printf("\nInitial battlefield conditions saved to ");
        printf("part1_A_initial.txt\n");
    }

    //Step - 2 Determine B will Sink

    printf("\n========================================\n");
    printf("STEP 2 - ESCORT SHIPS ATTACK BATTLESHIP\n");
    printf("========================================\n");

    tolerance = 1.0;

    for (int i = 0; i < N; i++)
    {
        escortHit[i] = 0;
        escortHitTime[i] = 0.0;
        escortHitAngle[i] = 0.0;
        escortHitVelocity[i] = 0.0;
    }

    for (int i = 0; i < N; i++)
    {

        eFiringVelocity = E[i].vMin + ((double)rand() / RAND_MAX) * (E[i].vMax - E[i].vMin);
        
        eFiringAngle = E[i].angleMin + ((double)rand() / RAND_MAX) * (E[i].angleMax - E[i].angleMin);

        eAngleRadians = eFiringAngle * M_PI / 180.0;

    //Horizontal Distance from E and B

        eDx = fabs(B.x - E[i].x);

    //Vertical Distance from E to B

        eDy = B.y - E[i].y;

        printf("\nE%d attacks Battleship:\n", E[i].id);

        printf("  Firing Velocity : %.2f m/s\n", eFiringVelocity);

        printf("  Firing Angle    : %.2f degrees\n", eFiringAngle);


    
       //Special case: E and B have the same X coordinate.

       //In this case the projectile travels vertically.

        if (eDx == 0)
        {  
            if (eDy >= 0 && eDy <=(eFiringVelocity * eFiringVelocity *
                            sin(eAngleRadians) * sin(eAngleRadians))/ (2 * 9.81))
            {
                eFlightTime = eDy / (eFiringVelocity * sin(eAngleRadians));

                printf("  Result           : HIT\n");

                printf("  Time to Hit      : %.2f seconds\n", eFlightTime);

                battleshipSunk = 1;
                sinkingEscortIndex = E[i].id;
                battleEndTime = eFlightTime;

                break;
            }
            else
            {
                printf("  Result           : MISS\n");
            }

            continue;
        }

        eFlightTime = eDx / (eFiringVelocity * cos(eAngleRadians));


    //Calculate projectile height at B's horizontal position.

        eProjectileY = E[i].y + eFiringVelocity * sin(eAngleRadians) *
                        eFlightTime - 0.5 * 9.81 * eFlightTime * eFlightTime;

        printf("  Projectile Y     : %.2f m\n", eProjectileY);

        printf("  Battleship Y     : %.2f m\n", B.y);


        if (fabs(eProjectileY - B.y) <= tolerance)
        {
            printf("  Result           : HIT\n");

            printf("  Time to Hit      : %.2f seconds\n", eFlightTime);

            battleshipSunk = 1;
            sinkingEscortIndex = E[i].id;
            battleEndTime = eFlightTime;

            break;
        }
        else
        {
            printf("  Result           : MISS\n");
        }
    }

//STEP 3A - BATTLESHIP IS SUNK

    if (battleshipSunk == 1)
    {
        FILE *finalFile;

        printf("\n========================================\n");
        printf("BATTLE RESULT\n");
        printf("========================================\n");

        printf("Battleship Status : SUNK\n");

        printf("Escort Ship that sank Battleship : E%d\n",sinkingEscortIndex);
        printf("Time of sinking : %.2f seconds\n",battleEndTime);


    //Save Final Battle Field Conditions

        finalFile = fopen("part1_A_final.txt", "w");

        if (finalFile == NULL)
        {
            printf("\nError: Could not create " "part1_A_final.txt\n");
        }
        else
        {
            fprintf(finalFile, "========================================\n");

            fprintf(finalFile, "ADVANCED NAVAL BATTLE SIMULATOR\n");

            fprintf(finalFile,"PART 1-A - FINAL CONDITIONS\n");

            fprintf(finalFile,"========================================\n\n");


        //Battle Result
            fprintf(finalFile, "BATTLE RESULT\n");

            fprintf(finalFile,"Battleship Status: SUNK\n");

            fprintf(finalFile,"Escort Ship that sank Battleship: E%d\n",sinkingEscortIndex);

            fprintf(finalFile,"Time of Sinking: %.2f seconds\n\n",battleEndTime);
            
            fprintf(finalFile,"FINAL BATTLESHIP CONDITIONS\n");

            fprintf(finalFile,"Type: %s\n",B.type);

            fprintf(finalFile,"Position: (%.2f, %.2f)\n",B.x,B.y);

            fprintf(finalFile,"Vmax: %.2f m/s\n\n",B.vMax);


        //Escort Ship information.
            fprintf(finalFile,"ESCORT SHIPS\n\n");

            for (int i = 0; i < N; i++)
            {
                fprintf(finalFile,"E%d\n",E[i].id);

                fprintf(finalFile,"Type: %s\n",E[i].type);

                fprintf(finalFile,"Position: (%.2f, %.2f)\n",E[i].x,E[i].y);

                fprintf(finalFile,"Vmin: %.2f m/s\n",E[i].vMin);

                fprintf(finalFile,"Vmax: %.2f m/s\n",E[i].vMax);

                fprintf(finalFile,"Minimum Angle: %.2f degrees\n",E[i].angleMin);

                fprintf(finalFile,"Maximum Angle: %.2f degrees\n",E[i].angleMax);

                fprintf(finalFile,"Angle Range: %.2f degrees\n",E[i].angleRange);

                fprintf(finalFile,"Impact Power: %.2f\n\n",E[i].impactPower);
            }

            fclose(finalFile);

            printf("\nFinal battlefield conditions saved to ");
            printf("part1_A_final.txt\n");
    }
}


//STEP 3B - BATTLESHIP SURVIVES
    else
    {
        FILE *finalFile;

        printf("\n========================================\n");
        printf("BATTLE RESULT\n");
        printf("========================================\n");

        printf("Battleship Status : SURVIVED\n");


        printf("\n========================================\n");
        printf("BATTLESHIP ATTACKS ESCORT SHIPS\n");
        printf("========================================\n");

        hitCount = 0;
        battleEndTime = 0.0;


        for (int i = 0; i < N; i++)
        {
        //Horizontal and vertical distances.

            dx = fabs(E[i].x - B.x);
            dy = E[i].y - B.y;

            printf("\nE%d:\n",E[i].id);


        //Special case:E and B have the same X coordinate.

            if (dx == 0)
            {
                if (dy >= 0 && dy <= (firingVelocity * firingVelocity) / (2 * 9.81))
                {
                    flightTime = dy / firingVelocity;

                    printf("  Firing Velocity : %.2f m/s\n",firingVelocity);

                    printf("  Firing Angle    : 90.00 degrees\n");

                    printf("  Result          : HIT\n");

                    hitCount++;

                    escortHit[i] = 1;
                    escortHitTime[i] = flightTime;
                    escortHitAngle[i] = 90.0;
                    escortHitVelocity[i] = firingVelocity;

                    if (flightTime > battleEndTime)
                    {
                        battleEndTime = flightTime;
                    }
                }
                else
                {
                    printf("  Result          : CANNOT REACH\n");
                }

                continue;
            }


        //Calculate the discriminant

            discriminant =
                (firingVelocity *
                firingVelocity *
                firingVelocity *
                firingVelocity)
                -
                9.81 *
                (9.81 * dx * dx +
                2 * dy *
                firingVelocity *
                firingVelocity);

            if (discriminant < 0)
            {
                printf("  Result          : CANNOT REACH\n");
                continue;
            }


        //Calculate the two possible firing angles.

            lowerAngle = atan((firingVelocity * firingVelocity -sqrt(discriminant))
                          /(9.81 * dx));

            upperAngle = atan((firingVelocity * firingVelocity + sqrt(discriminant))
                          /(9.81 * dx));

            if (lowerAngle >= 0 && lowerAngle <= M_PI / 2.0)
            {
                angleRadians = lowerAngle;
            }
            else if (upperAngle >= 0 && upperAngle <= M_PI / 2.0)
            {
                angleRadians = upperAngle;
            }
            else
            {
                printf("  Result          : CANNOT REACH\n");
                continue;
            }


            flightTime = dx / (firingVelocity * cos(angleRadians));

            projectileY = B.y + firingVelocity * sin(angleRadians) * flightTime -
                          0.5 * 9.81 * flightTime * flightTime;


            printf("  Firing Velocity : %.2f m/s\n",firingVelocity);

            printf("  Firing Angle    : %.2f degrees\n", angleRadians * 180.0 / M_PI);

            printf("  Target          : (%.2f, %.2f)\n",E[i].x,E[i].y);

            printf("  Projectile Y    : %.2f m\n",projectileY);
            
            //Check whether the projectile hits E.

            if (fabs(projectileY - E[i].y) <= tolerance)
            {
                printf("  Result          : HIT\n");
                hitCount++;

                escortHit[i] = 1;

                escortHitTime[i] = flightTime;

                escortHitAngle[i] = angleRadians * 180.0 / M_PI;

                escortHitVelocity[i] = firingVelocity;
                
                if (flightTime > battleEndTime)
                {
                    battleEndTime = flightTime;
                }
            }
            else
            {
                printf("  Result          : MISS\n");
            }
        }

        //Display Final Battle Results

        printf("\n========================================\n");
        printf("FINAL BATTLE RESULTS\n");
        printf("========================================\n");

        printf("Battleship Status : SURVIVED\n");

        printf("Number of Escort Ships Hit by B : %d\n",hitCount);

        printf("Battle End Time : %.2f seconds\n",battleEndTime);


    //Save final battlefield conditions.

        finalFile = fopen("part1_A_final.txt", "w");

        if (finalFile == NULL)
        {
            printf("\nError: Could not create " "part1_A_final.txt\n");
        }
        else
        {
            fprintf(finalFile, "========================================\n");

            fprintf(finalFile,"ADVANCED NAVAL BATTLE SIMULATOR\n");

            fprintf(finalFile,"PART 1-A - FINAL CONDITIONS\n");

            fprintf(finalFile,"========================================\n\n");


        //Battle result.
            fprintf(finalFile,"BATTLE RESULT\n");

            fprintf(finalFile,"Battleship Status: SURVIVED\n");

            fprintf(finalFile,"Number of Escort Ships Hit by B: %d\n",hitCount);

            fprintf(finalFile,"Battle End Time: %.2f seconds\n\n",battleEndTime);


        //Details of Escort Ships hit by B.

            fprintf(finalFile,"ESCORT SHIPS HIT BY B\n\n");

            if (hitCount == 0)
            {
                fprintf(finalFile,"No Escort Ships were hit.\n\n");
            }
            else
            {
                for (int i = 0; i < N; i++)
                {
                    if (escortHit[i] == 1)
                    {
                        fprintf(finalFile,"E%d\n",E[i].id);

                        fprintf(finalFile,"Type: %s\n",E[i].type);

                        fprintf(finalFile,"Position: (%.2f, %.2f)\n",E[i].x,E[i].y);

                        fprintf(finalFile,"Time to Hit: %.2f seconds\n",escortHitTime[i]);

                        fprintf(finalFile,"Firing Velocity: %.2f m/s\n",escortHitVelocity[i]);

                        fprintf(finalFile,"Firing Angle: %.2f degrees\n\n",escortHitAngle[i]);
                    }
                }
            }

            fprintf(finalFile,    "FINAL BATTLESHIP CONDITIONS\n");

            fprintf(finalFile,"Type: %s\n",B.type);

            fprintf(finalFile,"Position: (%.2f, %.2f)\n",B.x,B.y);

            fprintf(finalFile,"Vmax: %.2f m/s\n\n",B.vMax);


        //Final Escort Ship conditions.

            fprintf(finalFile,"FINAL ESCORT SHIP CONDITIONS\n\n");

            for (int i = 0; i < N; i++)
            {
                fprintf(finalFile,"E%d - Type: %s - " "Position: (%.2f, %.2f)\n",
                    E[i].id,
                    E[i].type,
                    E[i].x,
                    E[i].y);
            }
            fclose(finalFile);

            printf("\nFinal battlefield conditions saved to ");
            printf("part1_A_final.txt\n");
        }
    }
    return 0;
}