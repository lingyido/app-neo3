from boilerplate_client.boilerplate_cmd import BoilerplateCommand

def test_version(backend, firmware):
    client = BoilerplateCommand(backend)
    assert client.get_version() == (0, 2, 0)
