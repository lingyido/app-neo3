from apps.neo_n3_cmd import Neo_n3_Command

def test_version(backend, firmware):
    client = Neo_n3_Command(backend)
    assert client.get_version() == (0, 2, 0)
