from itertools import permutations


def is_matching_stabile(men_preferences, women_preferences, matching):
    for man in men_preferences:
        actual_woman = matching.get(man)

        if actual_woman is not None:
            # Trova le donne che l'uomo preferisce all'attuale compagna
            preferred_women = men_preferences[man][:men_preferences[man].index(str(actual_woman))]

            for woman in preferred_women:
                # Verifica se la donna preferisce l'uomo all'attuale compagno
                for m, w in matching.items():
                    if str(w) == woman:
                        actual_man_of_woman = m
                        if women_preferences[woman].index(str(man)) < women_preferences[woman].index(str(actual_man_of_woman)):
                            # L'uomo è preferito all'attuale compagno di woman
                            return False
    return True


def generate_all_stable_matchings(men_preferences, women_preferences, n):
    men = list(men_preferences.keys())
    women = list(women_preferences.keys())
    all_possible_matchings = list(permutations(range(n), n))
    stable_matchings = []

    for matching in all_possible_matchings:
        current_matching = dict(zip(men, matching))

        if is_matching_stabile(men_preferences, women_preferences, current_matching):
            stable_matchings.append(current_matching)

    return stable_matchings

def main():
    """
    men_preferences = {
        '0': ['4', '6', '0', '1', '5', '7', '3', '2'],
        '1': ['1', '2', '6', '4', '3', '0', '7', '5'],
        '2': ['7', '4', '0', '3', '5', '1', '2', '6'],
        '3': ['2', '1', '6', '3', '0', '5', '7', '4'],
        '4': ['6', '1', '4', '0', '2', '5', '7', '3'],
        '5': ['0', '5', '6', '4', '7', '3', '1', '2'],
        '6': ['1', '4', '6', '5', '2', '3', '7', '0'],
        '7': ['2', '7', '3', '4', '6', '1', '5', '0'],
    }

    women_preferences = {
        '0': ['4', '2', '6', '5', '0', '1', '7', '3'],
        '1': ['7', '5', '2', '4', '6', '1', '0', '3'],
        '2': ['0', '4', '5', '1', '3', '7', '6', '2'],
        '3': ['7', '6', '2', '1', '3', '0', '4', '5'],
        '4': ['5', '3', '6', '2', '7', '0', '1', '4'],
        '5': ['1', '7', '4', '3', '5', '2', '6', '0'],
        '6': ['6', '4', '1', '0', '7', '5', '3', '2'],
        '7': ['6', '3', '0', '4', '1', '2', '5', '7'],
    }
    n = 8
    """

    men_preferences = {
        '0': ['2', '0', '1'],
        '1': ['0', '1', '2'],
        '2': ['1', '0', '2'],
    }

    women_preferences = {
        '0': ['2', '0', '1'],
        '1': ['0', '1', '2'],
        '2': ['1', '2', '0'],
    }
    n = 3

    all_stable_matchings = generate_all_stable_matchings(men_preferences, women_preferences, n)
    for matching in all_stable_matchings:
        res_element = []
        for i in range(n):
            res_element.append(matching.get(str(i)))
        print(res_element)


if __name__ == '__all_stable_matchings__':
    main()
