/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dzuntini <dzuntini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 00:00:00 by dzuntini          #+#    #+#             */
/*   Updated: 2026/04/02 00:00:00 by dzuntini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include "malloc.h"

int main(void)
{
	ft_printf("=== Tests unitaires malloc ===\n");
	
	void *ptr1 = malloc(10);
    void *ptr2 = malloc(20);
    void *ptr3 = malloc(30);

	puts("toto");
    ft_printf("Allocated memory at: %p\n", ptr1);
    ft_printf("Allocated memory at: %p\n", ptr2);
    ft_printf("Allocated memory at: %p\n", ptr3);

    free(ptr1);
    free(ptr2);
    free(ptr3);
	
	ft_printf("Tests terminés\n");
	return (0);
}
