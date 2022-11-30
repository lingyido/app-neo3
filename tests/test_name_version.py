from boilerplate_client.boilerplate_cmd import BoilerplateCommand

def test_get_app_and_version(backend, backend_name, firmware):
    client = BoilerplateCommand(backend)

    app_name, version = client.get_app_and_version()

    if backend_name == "speculos":
        # for now, Speculos always returns app:1.33.7, so this test is not very meaningful on it
        assert app_name == "app"
        assert version == "1.33.7"
    else:
        assert app_name == "NEO N3"
        assert version == "0.1.2"


