#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PATIENTS 5000

typedef struct {
    char id[20];
    int severity;
    int arrival;
    int treatment;
    char spec[20];
    int treated;
} Patient;

typedef struct {
    char id[20];
    char spec[20];
    int free_time;
} Doctor;

typedef struct {
    char patient_id[20];
    char doctor_id[20];
    int start_time;
    int end_time;
} Schedule;

Patient patients[MAX_PATIENTS];
Schedule schedules[MAX_PATIENTS];

int patient_count = 0;
int schedule_count = 0;

Doctor doctors[3] = {
    {"Doctor_T","TRAUMA",0},
    {"Doctor_C","CARDIO",0},
    {"Doctor_G","GENERAL",0}
};

int compatible(Patient p, Doctor d)
{
    if(strcmp(d.spec,"GENERAL")==0) return 1;
    if(strcmp(p.spec,d.spec)==0) return 1;
    return 0;
}

double priority_score(Patient p, int current_time)
{
    int waiting = current_time - p.arrival;
    if(waiting < 0) waiting = 0;

    return (double)(p.severity * (waiting + 1)) / p.treatment;
}

void to_upper(char *s)
{
    for(int i=0;s[i];i++)
        s[i] = toupper(s[i]);
}

void load_patients()
{
    FILE *fp = fopen("patients200.csv","r");

    if(!fp){
        printf("patients200.csv not found\n");
        exit(1);
    }

    char line[256];

    fgets(line,256,fp); // skip header

    while(fgets(line,256,fp))
    {
        Patient p;

        sscanf(line,"%[^,],%d,%d,%d,%s",
               p.id,&p.severity,&p.arrival,&p.treatment,p.spec);

        to_upper(p.spec);

        p.treated = 0;
        patients[patient_count++] = p;
    }

    fclose(fp);
}

int all_treated()
{
    for(int i=0;i<patient_count;i++)
        if(!patients[i].treated)
            return 0;

    return 1;
}

void write_json(int total_risk)
{
    FILE *out = fopen("submission.json","w");

    fprintf(out,"{\n");
    fprintf(out,"  \"treatments\": [\n");

    for(int i=0;i<schedule_count;i++)
    {
        fprintf(out,
        "    {\"patient_id\":\"%s\",\"doctor_id\":\"%s\",\"start_time\":%d,\"end_time\":%d}",
        schedules[i].patient_id,
        schedules[i].doctor_id,
        schedules[i].start_time,
        schedules[i].end_time);

        if(i < schedule_count-1)
            fprintf(out,",\n");
        else
            fprintf(out,"\n");
    }

    fprintf(out,"  ],\n");
    fprintf(out,"  \"estimated_total_risk\": %d\n", total_risk);
    fprintf(out,"}\n");

    fclose(out);
}

int main()
{
    load_patients();

    int time = 0;

    while(!all_treated())
    {
        for(int d=0; d<3; d++)
        {
            if(doctors[d].free_time > time)
                continue;

            double best_score = -1;
            int best_index = -1;

            for(int i=0;i<patient_count;i++)
            {
                if(patients[i].treated) continue;
                if(patients[i].arrival > time) continue;
                if(!compatible(patients[i],doctors[d])) continue;

                double score = priority_score(patients[i],time);

                if(score > best_score)
                {
                    best_score = score;
                    best_index = i;
                }
            }

            if(best_index != -1)
            {
                Patient *p = &patients[best_index];

                int start = time;
                if(start < p->arrival)
                    start = p->arrival;

                int end = start + p->treatment;

                doctors[d].free_time = end;

                strcpy(schedules[schedule_count].patient_id,p->id);
                strcpy(schedules[schedule_count].doctor_id,doctors[d].id);

                schedules[schedule_count].start_time = start;
                schedules[schedule_count].end_time = end;

                schedule_count++;

                p->treated = 1;
            }
        }

        int next_time = 100000000;

        for(int d=0; d<3; d++)
            if(doctors[d].free_time > time && doctors[d].free_time < next_time)
                next_time = doctors[d].free_time;

        for(int i=0;i<patient_count;i++)
            if(!patients[i].treated &&
               patients[i].arrival > time &&
               patients[i].arrival < next_time)
                next_time = patients[i].arrival;

        if(next_time == 100000000)
            break;

        time = next_time;
    }

    int total_risk = 0;

    for(int i=0;i<schedule_count;i++)
    {
        for(int j=0;j<patient_count;j++)
        {
            if(strcmp(schedules[i].patient_id,patients[j].id)==0)
            {
                int wait = schedules[i].start_time - patients[j].arrival;

                if(wait < 0) wait = 0;

                total_risk += patients[j].severity * wait;
            }
        }
    }

    write_json(total_risk);

    printf("submission.json generated successfully\n");

    return 0;
}