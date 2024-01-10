void print_map_table()
{
    int i;
    for (i = E_ID; i < HI_E_TE_NN2 + 1; ++i) {
        switch (i) {
            case E_ID:
                printf("Device type[%d]: %s\n", E_ID, get_device_type_name(E_ID));
                break;
            case E_NAME:
                printf("Device type[%d]: %s\n", E_NAME, get_device_type_name(E_NAME));
                break;
            case E_SIZE:
                printf("Device type[%d]: %s\n", E_SIZE, get_device_type_name(E_SIZE));
                break;
            case E_test:
                printf("Device type[%d]: %s\n", E_test, get_device_type_name(E_test));
                break;
            case E_test2:
                printf("Device type[%d]: %s\n", E_test2, get_device_type_name(E_test2));
                break;
            case HI_E_TE__NN:
                printf("Device type[%d]: %s\n", HI_E_TE__NN, get_device_type_name(HI_E_TE__NN));
                break;
            case HI_E_TE_NN2:
                printf("Device type[%d]: %s\n", HI_E_TE_NN2, get_device_type_name(HI_E_TE_NN2));
                break;
            default:
                printf("Unknown device type\n");
                break;
        }
    }
}

int main()
{
    printf("\nPrinf mapping table(Use example):\n");
    print_map_table();
    return 0;
}
