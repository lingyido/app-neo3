from neo3crypto import ECCCurve, ECPoint
from boilerplate_client.boilerplate_cmd import BoilerplateCommand

def test_get_public_key(backend, firmware):
    client = BoilerplateCommand(backend)
    pub_key = client.get_public_key(
        bip44_path="m/44'/888'/0'/0/0",
        display=False
    )  # type: bytes, bytes

    assert len(pub_key) == 65
    assert ECPoint(pub_key, ECCCurve.SECP256R1, validate=True)

    pub_key2 = client.get_public_key(
        bip44_path="m/44'/888'/1'/0/0",
        display=False
    )  # type: bytes, bytes

    assert len(pub_key2) == 65
    assert ECPoint(pub_key2, ECCCurve.SECP256R1, validate=True)
