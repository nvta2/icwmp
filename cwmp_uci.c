#include "cwmp_uci.h"

static inline bool cwmp_check_section_name(const char *str, bool name)
{
	if (!*str)
		return false;
	for (; *str; str++) {
		unsigned char c = *str;
		if (isalnum(c) || c == '_')
			continue;
		if (name || (c < 33) || (c > 126))
			return false;
	}
	return true;
}

static void cwmp_uci_list_init(struct uci_list *ptr)
{
	ptr->prev = ptr;
	ptr->next = ptr;
}

static void cwmp_uci_list_add(struct uci_list *head, struct uci_list *ptr)
{
	head->prev->next->prev = ptr;
	ptr->prev = head->prev;
	ptr->next = head->prev->next;
	head->prev->next = ptr;
}

int cwmp_uci_lookup_ptr(struct uci_context *ctx, struct uci_ptr *ptr, char *package, char *section, char *option, char *value)
{
	/*value*/
	ptr->value = value;

	/*package*/
	if (!package)
		return -1;
	ptr->package = package;

	/*section*/
	if (!section || !section[0]) {
		ptr->target = UCI_TYPE_PACKAGE;
		goto lookup;
	}
	ptr->section = section;
	if (ptr->section &&  !cwmp_check_section_name(ptr->section , true))
		ptr->flags |= UCI_LOOKUP_EXTENDED;

	/*option*/
	if (!option || !option[0]) {
		ptr->target = UCI_TYPE_SECTION;
		goto lookup;
	}
	ptr->target = UCI_TYPE_OPTION;
	ptr->option = option;

lookup:
	if (uci_lookup_ptr(ctx, ptr, NULL, true) != UCI_OK || !UCI_LOOKUP_COMPLETE) {
		return -1;
	}
	return 0;
}

int cwmp_uci_get_option_value_list(char *package, char *section, char *option, struct list_head *list)
{
	struct uci_element *e;
	struct uci_ptr ptr = {0};
	struct uci_context *uci_ctx = uci_alloc_context();
	struct config_uci_list      *uci_list_elem;
	int size = 0;
	//*value = NULL;

	if (cwmp_uci_lookup_ptr(uci_ctx, &ptr, package, section, option, NULL)) {
		uci_free_context(uci_ctx);
		return -1;
	}

	if (ptr.o == NULL) {
		uci_free_context(uci_ctx);
		return -1;
	}

    if (ptr.o->type == UCI_TYPE_LIST) {
        uci_foreach_element(&ptr.o->v.list, e)
        {
            if((e != NULL)&&(e->name))
            {
                uci_list_elem = calloc(1,sizeof(struct config_uci_list));
                if(uci_list_elem == NULL)
                {
                    uci_free_context(uci_ctx);
                    return -1;
                }
                uci_list_elem->value = strdup(e->name);
                list_add_tail (&(uci_list_elem->list), list);
                size++;
            }
            else
            {
                uci_free_context(uci_ctx);
                return size;
            }
        }
	}

    uci_free_context(uci_ctx);
	return size;
}

void free_config_uci_list(struct list_head *conf_uci_list)
{
	struct config_uci_list *conf_uci;
	while (conf_uci_list->next != conf_uci_list) {
		conf_uci = list_entry(conf_uci_list->next, struct config_uci_list, list);
		if(conf_uci->value != NULL) {
			free(conf_uci->value);
			conf_uci->value = NULL;
		}
		list_del(&conf_uci->list);
	}
}
