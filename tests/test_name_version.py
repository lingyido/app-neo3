from apps.neo_n3_cmd import Neo_n3_Command

def test_get_app_and_version(backend, backend_name, firmware):
    client = Neo_n3_Command(backend)

    app_name, version = client.get_app_and_version()

    if backend_name == "speculos":
        # for now, Speculos always returns app:1.33.7, so this test is not very meaningful on it
        assert app_name == "app"
        assert version == "1.33.7"
    else:
        assert app_name == "NEO N3"
        assert version == "0.1.2"


