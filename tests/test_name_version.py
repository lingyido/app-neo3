from apps.neo_n3_cmd import Neo_n3_Command

def test_get_app_and_version(backend, backend_name, firmware):
    client = Neo_n3_Command(backend)

    app_name, version = client.get_app_and_version()

    assert app_name == "NEO N3"
    assert version == "0.3.1"
