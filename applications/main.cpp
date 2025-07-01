#include <System>
#include <Solid>
#include <Axion>

int main(int argc, char *argv[])
{
    Application *app = new Application(argc, argv);

    ApplicationUpdater::init();
    LocaleSettings::init();

    AxionHelper::onRestartAccepted(Power::restart);
    AxionHelper::onRebootAccepted(Power::reboot);

    return app->run("L00_Launcher", "AW00_Launcher");
}
