#include <assert.h>

#include "pnode.h"
#include "cfg.h"

real_t
pnode_mem_percentage(pnode_t *pnode)
{
	assert(config.max_mem >= config.min_mem);

	real_t m = pnode->mem;
	if (m < config.min_mem)
		return 0;
	if (m > config.max_mem)
		return 1;

	return (m - config.min_mem) / (config.max_mem - config.min_mem);
}


real_t
pnode_cpu_percentage(pnode_t *pnode)
{
	assert(config.max_cpu >= config.min_cpu);

	real_t m = pnode->cpu;
	if (m < config.min_cpu)
		return 0;
	if (m > config.max_cpu)
		return 1;

	return (m - config.min_cpu) / (config.max_cpu - config.min_cpu);
}

