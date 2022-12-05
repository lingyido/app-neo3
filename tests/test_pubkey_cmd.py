from neo3crypto import ECCCurve, ECPoint
from apps.neo_n3_cmd import Neo_n3_Command

def test_get_public_key(backend, firmware):
    client = Neo_n3_Command(backend)
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
