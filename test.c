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
	printf("=== Tests unitaires malloc ===\n");
	
	 void *ptr1 = malloc(10);
    void *ptr2 = malloc(20);
    void *ptr3 = malloc(30);

    printf("Allocated memory at: %p\n", ptr1);
    printf("Allocated memory at: %p\n", ptr2);
    printf("Allocated memory at: %p\n", ptr3);

    free(ptr1);
    free(ptr2);
    free(ptr3);
	
	printf("Tests terminés\n");
	return (0);
}
