Code de production de la classe UISS

Ci-dessous un main de test (pas propre, sans méthodes actives)

```
int main()
{
    uiss_launchSS();
    printf("launched\n");
    uiss_display();
    printf("wait 2 sec\n");
    sleep(2);
    uiss_updateCamState(true);
    sleep(1);
    uiss_updateCamState(false);
    sleep(1);
    uiss_updateDoorState(true);
    sleep(1);
    uiss_updateDoorState(false);
    sleep(1);
    uiss_signalFaceNotAllowed();
}
```