#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "libnmcli.h"

int main(int argc, char **argv)
{
    char *output = NULL;
    char *error = NULL;
    int result;

    printf("nmcli Library Example\n");
    printf("Version: %s\n\n", nmcli_get_version());

    printf("Library ready to use\n");

    /* Example 1: Simple command execution */
    /**/
    printf("\n=== Example 1: Simple command ===\n");
    char *args1[] = {"nmcli", "device", "status"};
    result = nmcli_execute(3, args1);
    printf("Result: %d\n", result);

    /* Example 2: Command with output capture */
    printf("\n=== Example 2: Command with output capture ===\n");
    char *args2[] = {"nmcli", "device", "status"};
    result = nmcli_execute_with_output(3, args2, &output, &error);
    printf("Result: %d\n", result);
    
    if (output && strlen(output) > 0) {
        printf("Output:\n%s\n", output);
    }
    
    if (error && strlen(error) > 0) {
        printf("Error:\n%s\n", error);
    }
    
    /* Example 3: Connection list */
    printf("\n=== Example 3: Connection list ===\n");
    char *args3[] = {"nmcli", "connection", "show"};
    result = nmcli_execute_with_output(3, args3, &output, &error);
    printf("Result: %d\n", result);
    
    if (output && strlen(output) > 0) {
        printf("Output:\n%s\n", output);
    }

    /* Clean up */
    g_free(output);
    g_free(error);

    printf("\nExample completed\n");
    return 0;
} 