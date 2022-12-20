from apps.neo_n3_cmd import Neo_n3_Command
from ragger.bip import calculate_public_key_and_chaincode, CurveChoice

def test_get_public_key(backend, firmware):
    client = Neo_n3_Command(backend)

    for path in ["m/44'/888'/0'/0/0", "m/44'/888'/1'/0/0", "m/44'/888'/10'/1/23"]:
        pub_key = client.get_public_key(bip44_path=path)
        ref_public_key, _ = calculate_public_key_and_chaincode(curve=CurveChoice.Nist256p1, path=path)
        assert pub_key.hex() == ref_public_key
