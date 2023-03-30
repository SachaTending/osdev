#include "common.h"
#include "printf.h"
#include "limine.h"
#include "logger.h"
struct limine_smp_request s {
    .id = LIMINE_SMP_REQUEST
};

typedef struct {
    bool avaible = false;
    void (*task)(struct limine_smp_info *) = NULL;
} tasks_t;

tasks_t tasks[64] = {0, NULL};
int cpus = 1;
void apic(struct limine_smp_info *smp) {
    LOG_START;printf("SMP: Started core %d\n", smp->processor_id);
    cpus++;
    while (true) {
        if (tasks[smp->processor_id].avaible) {
            tasks[smp->processor_id].task(smp);
            tasks[smp->processor_id].avaible = false;
            tasks[smp->processor_id].task = NULL;
        }
    }
}

cmab void smp_init() {
    log("Cores: ");printf("%d BSP ID: %d\n",s.response->cpu_count, s.response->bsp_lapic_id);
    //for (int i=s.response->bsp_lapic_id;i++;i<s.response->cpu_count) s.response->cpus[i]->goto_address=NULL;
    log("Starting cores...\n");
    while (cpus != s.response->cpu_count) {
        for (int i=0;i<s.response->cpu_count;i++) {
            //log("");printf("Starting up core %d...\n", s.response->cpus[i]->processor_id);
            s.response->cpus[i]->goto_address = apic;
        }
    }
}